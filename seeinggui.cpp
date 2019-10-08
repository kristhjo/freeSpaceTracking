#include "seeinggui.h"
#include "ui_seeinggui.h"

SeeingGui::SeeingGui(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::SeeingGui)
{
    ui->setupUi(this);
    this->isProcessing = false;
    this->ui->CheckB_Yes->setChecked(true);
    this->DIMM_thread = nullptr;
    this->storeImages = true;
    this->measurementType = this->ui->ComboB_MeasurementType->currentText();
    this->ui->LE_SampleSize->setText("50");
    this->ui->SB_WindowingRadius->setMinimum(1);
    this->ui->SB_WindowingRadius->setMaximum(10);
    this->ui->SB_WindowingRadius->setValue(1);

    QObject::connect(this->ui->LE_SampleSize,&QLineEdit::textChanged,this,&SeeingGui::setSecPerDataPoint,Qt::UniqueConnection);
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(replotSeeing()));
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(updateSeeingPanel()));
    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
}

SeeingGui::~SeeingGui(){
    delete ui;
}

void SeeingGui::startMeasurement(){
    this->setMeasurementSettings();
    this->startOfMeasurement = time(nullptr);
    if(!this->baseDirectory.mkpath(this->directoryPath)) {  //Creates a base directory for the measurement where results are stored.
        this->displayMessage("Error creating basedirectory: " + this->directoryPath, true);
        return;
    }

    this->createParameterFile();
    this->initPlots();
    this->isMeasuringSeeing->store(true, std::memory_order_release);
     this->testStream.open("testData.dat",std::fstream::out|std::fstream::app);
    if (this->measurementType == "DIMM"){
        this->DIMM_thread = std::make_unique<std::thread>(&SeeingGui::DIMM,this); //Initiates a real-time image processing loop while images are captured.
    }
    else if (this->measurementType == "Gaussian"){
        this->displayMessage("No support for Gaussian seeing measurements at this time.");
        this->Gauss_thread = std::make_unique<std::thread>(&SeeingGui::Gaussian,this); //Initiates a real-time image processing loop while images are captured.
        return;
    }
}

void SeeingGui::stopMeasurement(){
    this->isMeasuringSeeing->store(false, std::memory_order_release);
    this->endOfMeasurement = time(nullptr);

    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        this->writeParametersToFile << "End of measurement: " << ctime(&this->endOfMeasurement) << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while opening file " + this->directoryPath + "SeeingParameters.txt", true);
    }

    while (this->isProcessing.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->displayMessage("Waiting for image processing to finish");
    }
    if (this->measurementType == "DIMM"){
        if(this->DIMM_thread->joinable()){
            this->DIMM_thread->join();
        }
        else {
            this->displayMessage("Error while closing DIMM thread \n", true);
        }
        this->writeResultsToFile();
    }
    else if(this->measurementType == "Gaussian"){
        if(this->Gauss_thread->joinable()){
            this->Gauss_thread->join();
        }
        else {
            this->displayMessage("Error while closing Gauss thread \n", true);
        }
        this->writeResultsToFile();
    }
}

void SeeingGui::DIMM(){
    unsigned int counter = 1;
    this->isProcessing.store(true, std::memory_order_release);
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){

        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->sampleSize){//When counter reaches the sample size, calculate and plot the seeing/fried values

            this->m_seeingValues.fried_x.push_back(this->getFriedParameter_x());
            this->m_seeingValues.fried_y.push_back(this->getFriedParameter_y());
            this->m_seeingValues.fried.push_back(( this->m_seeingValues.fried_x.last() + this->m_seeingValues.fried_y.last() )/2.0);
            this->m_seeingValues.seeing_x.push_back(getSeeingFromFried(this->m_seeingValues.fried_x.last(), this->m_seeingParams.wavelength));
            this->m_seeingValues.seeing_y.push_back(getSeeingFromFried(this->m_seeingValues.fried_y.last(), this->m_seeingParams.wavelength));
            this->m_seeingValues.seeing.push_back( (this->m_seeingValues.seeing_x.last() + this->m_seeingValues.seeing_y.last() )/2.0);//    getSeeingFromFried(this->m_seeingValues.fried.last(), this->m_seeingParams.wavelength));
            emit newSeeingValues(); // updates the plots and display in gui
            counter = 1; //reset counter and empty m_DIMMsample
            //this->testStream << time(nullptr) << "\t" << this->m_seeingValues.fried_x.last() << "\t" << this->m_seeingValues.fried_y.last() << "\n";
            //this->testStream.flush();
            this->m_DIMMsample = {};
        }

        if (counter == 1){//At the beginning of a sample, store the timestamp. If storeImages = true, create a folder where images can be stored.
            this->m_seeingValues.timestamps.push_back(this->m_imageContainer->startTime);
            if(this->storeImages){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                if (!this->baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
        }

        if (!this->m_imageContainer->imgQueue.empty()){ //If there are images queued in the m_imagecontainer, process the oldest one.
            cv::Mat img = this->m_imageContainer->imgQueue.front();
            if (this->storeImages){
                if(!cv::imwrite( this->directoryPath.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
            }

            if (this->debug){
                this->debugDIMM(img, counter);
            }
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
            cv::Point spotDist = imageprocessing::getSpotSeparation(img, this->m_seeingParams.pxWindowRadius);
            if (spotDist.x == 10000){
                std::cout << "skipped image due to no valid centroid" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            else{
                this->m_DIMMsample.spotSeparation.push_back(spotDist);
            }
            this->m_imageContainer->removeFirstImg();

        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            emit newMessage("Waiting for new images");
            continue;
        }
        counter++;
    }
    emit newMessage("Finished seeing calculations");
    this->isProcessing.store(false, std::memory_order_release);
}

void SeeingGui::debugDIMM(cv::Mat image, int label){
    cv::Mat img1, img2, croppedImg1, croppedImg2;
    img1 = image(cv::Rect(0, 0, image.cols/2, image.rows)); //splits the original image in left and right halves. This assumes the spots are distributed this way, and not for example top-bottom.
    img2 = image(cv::Rect(image.cols/2, 0, image.cols/2, image.rows));
    imageprocessing::cropWindow(img1, croppedImg1, this->m_seeingParams.pxWindowRadius);
    imageprocessing::cropWindow(img2, croppedImg2, this->m_seeingParams.pxWindowRadius);
    cv::Point c1, c2;
    c1 = imageprocessing::findCentroid(croppedImg1);
    c2 = imageprocessing::findCentroid(croppedImg2);
    cv::circle(croppedImg1,c1,5,cv::Scalar(128,0,0),-1); //creates circle on img following the centroid.
    cv::circle(croppedImg2,c2,5,cv::Scalar(128,0,0),-1); //creates circle on img following the centroid.
    cv::hconcat(croppedImg1, croppedImg2, croppedImg1);
    if(!cv::imwrite( this->directoryPath.toStdString() + this->currentSubFolderName.toStdString()  + "/debug_img_" + std::to_string(label) + ".jpg", croppedImg1)){
        emit newMessage("Failed to save debug image", true);
    }
}

void SeeingGui::Gaussian(){
    unsigned int counter = 1;
    this->isProcessing.store(true, std::memory_order_release);
    datacontainers::gaussianFitParams tempParams;
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){

        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->sampleSize){//When counter reaches the sample size, calculate and plot the seeing/fried values

            this->m_seeingValues.seeing_x.push_back(this->m_GaussSample.FWHM_x());
            this->m_seeingValues.seeing_y.push_back(this->m_GaussSample.FWHM_y());

            this->m_seeingValues.fried_x.push_back(this->getFriedFromSeeing(this->m_seeingValues.seeing_x.last(), this->m_seeingParams.wavelength));
            this->m_seeingValues.fried_y.push_back(this->getFriedFromSeeing(this->m_seeingValues.seeing_y.last(), this->m_seeingParams.wavelength));
            this->m_seeingValues.fried.push_back(( this->m_seeingValues.fried_x.last() + this->m_seeingValues.fried_y.last() )/2.0);
            this->m_seeingValues.seeing.push_back( (this->m_seeingValues.seeing_x.last() + this->m_seeingValues.seeing_y.last() )/2.0);//    getSeeingFromFried(this->m_seeingValues.fried.last(), this->m_seeingParams.wavelength));
            emit newSeeingValues(); // updates the plots and display in gui
            counter = 1; //reset counter and empty m_DIMMsample

            this->m_GaussSample = {};
        }

        if (counter == 1){//At the beginning of a sample, store the timestamp. If storeImages = true, create a folder where images can be stored.
            this->m_seeingValues.timestamps.push_back(this->m_imageContainer->startTime);
            if(this->storeImages){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                if (!this->baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
        }

        if (!this->m_imageContainer->imgQueue.empty()){ //If there are images queued in the m_imagecontainer, process the oldest one.
            cv::Mat img = this->m_imageContainer->imgQueue.front();
            if (this->storeImages){
                if(!cv::imwrite( this->directoryPath.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
            }
            if (this->debug){
                this->debugGaussian(img, counter);
            }
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
            tempParams = imageprocessing::getGaussianFitParams(img, this->m_seeingParams.pxWindowRadius);
            if (tempParams.intensitymax < 10){
                std::cout << "skipped image due to low intensity" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            else if (tempParams.numSaturatedPixels > 1){
                std::cout << "skipped image due to " <<  tempParams.numSaturatedPixels << " saturated pixels" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            else{
                this->m_GaussSample.fitParams.push_back(tempParams);
            }
            this->m_imageContainer->removeFirstImg();

        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            emit newMessage("Waiting for new images");
            continue;
        }
        counter++;
    }
    emit newMessage("Finished seeing calculations");
    this->isProcessing.store(false, std::memory_order_release);
}

void SeeingGui::debugGaussian(cv::Mat image, int label){
    cv::Mat gaussFitImg, croppedImg;
    datacontainers::gaussianFitParams fitParams = imageprocessing::getGaussianFitParams(image, this->m_seeingParams.pxWindowRadius);
    imageprocessing::cropWindow(image, croppedImg, this->m_seeingParams.pxWindowRadius);
    imageprocessing::drawGaussian(gaussFitImg, fitParams);
    cv::hconcat(croppedImg, gaussFitImg, croppedImg);
    if(!cv::imwrite( this->directoryPath.toStdString() + this->currentSubFolderName.toStdString()  + "/debug_img_" + std::to_string(label) + ".jpg", croppedImg)){
        emit newMessage("Failed to save debug image", true);
    }
}


void SeeingGui::createParameterFile(){
    this->parameterFile.setFileName(this->directoryPath + "SeeingParameters.txt");
    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        this->writeParametersToFile.setDevice(&this->parameterFile);
        this->writeParametersToFile << "Seeing Measurement Parameters" << "\n";
        this->writeParametersToFile << "Start of measurement: " << ctime(&this->startOfMeasurement) << "\n";
        this->writeParametersToFile << "Measurement type: " << this->measurementType << "\n";
        this->writeParametersToFile << "Sample size: " << this->sampleSize << "\n";
        this->writeParametersToFile << "Exposure time: " << this->m_seeingParams.exposureTime << "\n";
        this->writeParametersToFile << "Window radius: " << this->m_seeingParams.pxWindowRadius << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while creating parameter file: " + this->directoryPath + "SeeingParameters.txt", true);
    }
}


void SeeingGui::writeResultsToFile(){
    this->resultsFile.setFileName(this->directoryPath + "Results.txt");
    if(this->resultsFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        this->writeResults.setDevice(&this->resultsFile);
        this->writeResults.setFieldAlignment(QTextStream::AlignLeft);
        this->writeResults << "Seeing Measurement Results: " << ctime(&this->startOfMeasurement) << "\n";
        this->writeResults.setFieldWidth(20);
        this->writeResults << "Timestamp" << "Fried" << "Seeing" << "Fried x" << "Fried y" << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
        for (int i = 0; i < this->m_seeingValues.fried.size(); i++){
            this->writeResults << this->m_seeingValues.timestamps.at(i) << this->m_seeingValues.fried.at(i) << this->m_seeingValues.seeing.at(i) <<  this->m_seeingValues.fried_x.at(i)<<  this->m_seeingValues.fried_y.at(i) <<qSetFieldWidth(0) << endl << qSetFieldWidth(20);
        }
        this->resultsFile.close();
    }
    else {
        this->displayMessage("Error while creating results file: " + this->directoryPath + "Results.txt", true);
    }
}

double SeeingGui::getFriedParameter(){
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_seeingParams.pxWidth,2)/(pow(this->m_seeingParams.receiverOptics.f3,2)*pow(this->m_seeingParams.receiverOptics.magnification,2));
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_seeingParams.pxHeight,2)/(pow(this->m_seeingParams.receiverOptics.f3,2)*pow(this->m_seeingParams.receiverOptics.magnification,2));
    double fried_x = pow(this->m_seeingParams.K_l*std::pow( angularVariance_x, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.maskApertureDiameter,-1./3.),3./5.);
    double fried_y = pow(this->m_seeingParams.K_t*std::pow( angularVariance_y, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.maskApertureDiameter,-1./3.),3./5.);
    return (fried_x + fried_y)/2.0;
}
double SeeingGui::getFriedParameter_x(){
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_seeingParams.pxWidth,2)/(pow(this->m_seeingParams.receiverOptics.f3,2)*pow(this->m_seeingParams.receiverOptics.magnification,2));
    double fried_x = pow(this->m_seeingParams.K_l*std::pow( angularVariance_x, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.maskApertureDiameter,-1./3.),3./5.);
    return fried_x;
}
double SeeingGui::getFriedParameter_y(){
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_seeingParams.pxHeight,2)/(pow(this->m_seeingParams.receiverOptics.f3,2)*pow(this->m_seeingParams.receiverOptics.magnification,2));
    double fried_y = pow(this->m_seeingParams.K_t*std::pow( angularVariance_y, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.maskApertureDiameter,-1./3.),3./5.);
    return fried_y;
}

double SeeingGui::getSeeingFromFried(double FriedParameter, double wavelength){
    return 0.98*wavelength/FriedParameter;
}
double SeeingGui::getFriedFromSeeing(double SeeingParameter, double wavelength){
    return 0.98*wavelength/SeeingParameter;
}

void SeeingGui::setMeasurementSettings(){
    this->m_seeingParams.pxWindowRadius = this->m_seeingParams.pxAiryZeros[this->ui->SB_WindowingRadius->value()]; //this->ui->SB_WindowingRadius->value();
    this->storeImages = this->ui->CheckB_Yes->isChecked();
    this->debug = this->ui->CheckB_Debug->isChecked();
    this->measurementType = this->ui->ComboB_MeasurementType->currentText();
    if(this->ui->LE_DirectoryPath->text().back() != "/"){   //corrects for a missing "/" at the end of the directory path in the user input
        this->directoryPath = this->ui->LE_DirectoryPath->text() + "/";
    }
    else {
        this->directoryPath = this->ui->LE_DirectoryPath->text();
    }
    this->baseDirectory.setPath(this->directoryPath);
    this->sampleSize = this->ui->LE_SampleSize->text().toUInt();
    this->m_imageContainer->sampleSize = this->sampleSize;
}

QString SeeingGui::timestampToFolderName(time_t timestamp){
    QDateTime t(QDateTime::fromTime_t(timestamp));
    return (t.toString(Qt::TextDate));
}

void SeeingGui::displayMessage(QString message, bool error){
    this->ui->TE_logSeeing->append(message);
    this->ui->TE_logSeeing->update();
    if (error){
        qDebug() << message;
    }
}

void SeeingGui::setSecPerDataPoint(){
    this->ui->LE_SecPerDataPoint->setText(QString::number(this->ui->LE_SampleSize->text().toInt()*this->m_seeingParams.exposureTime*0.000001, this->displayFormat, 2));
}
void SeeingGui::replotSeeing(){

    //First convert fried and seeing values to QCPGraphData format.
    QCPGraphData dataPoint(this->m_seeingValues.timestamps.last(), this->m_seeingValues.fried.last()*1e2);
    this->m_seeingValues.friedData.push_back(dataPoint);
    dataPoint.value = this->m_seeingValues.fried_x.last()*1e2;
    this->m_seeingValues.friedData_x.push_back(dataPoint);
    dataPoint.value = this->m_seeingValues.fried_y.last()*1e2;
    this->m_seeingValues.friedData_y.push_back(dataPoint);
    dataPoint.value = getSeeingFromFried(this->m_seeingValues.fried.last(), this->m_seeingParams.wavelength)*1e6;
    this->m_seeingValues.seeingData.push_back(dataPoint);

    this->ui->FriedPlot->yAxis->setRange(0, this->m_seeingValues.maxFried()*1.5);
    this->ui->FriedPlot->rescaleAxes();
    this->ui->FriedPlot->graph(0)->data()->set(this->m_seeingValues.friedData_y);
    this->ui->FriedPlot->graph(1)->data()->set(this->m_seeingValues.friedData_x);

    this->m_seeingValues.avgFriedPlot.clear();
    QCPGraphData dataPoint2(this->m_seeingValues.timestamps.first(), this->m_seeingValues.meanFried());
    this->m_seeingValues.avgFriedPlot.push_back(dataPoint2);
    dataPoint2.key = this->m_seeingValues.timestamps.last();
    this->m_seeingValues.avgFriedPlot.push_back(dataPoint2);
    this->ui->FriedPlot->graph(2)->data()->set(this->m_seeingValues.avgFriedPlot);
    this->avgFriedLabel->setText("Average Fried parameter: " + QString::number(dataPoint2.value, this->displayFormat, this->displayPrecision));

    this->ui->FriedPlot->replot();
    this->ui->FriedPlot->savePdf(this->directoryPath + "FriedPlot.pdf" );

    this->ui->SeeingPlot->yAxis->setRange(0, this->m_seeingValues.maxSeeing()*1.5);
    this->ui->SeeingPlot->rescaleAxes();
    this->ui->SeeingPlot->graph(0)->data()->set(this->m_seeingValues.seeingData);

    this->ui->SeeingPlot->replot();
    this->ui->SeeingPlot->savePdf( this->directoryPath + "SeeingPlot.pdf" );
}

void SeeingGui::updateSeeingPanel(){
    //fried parameter in cm, seeing in micro rad
    this->ui->LE_FriedMax->setText(QString::number(this->m_seeingValues.maxFried()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMin->setText(QString::number(this->m_seeingValues.minFried()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedNow->setText(QString::number(this->m_seeingValues.fried.last()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMean->setText(QString::number(this->m_seeingValues.meanFried()*1e2, this->displayFormat, this->displayPrecision));


    this->ui->LE_FriedMean_x->setText(QString::number(this->m_seeingValues.meanFried_x()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedNow_x->setText(QString::number(this->m_seeingValues.fried_x.last()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMax_x->setText(QString::number(this->m_seeingValues.maxFried_x()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMin_x->setText(QString::number(this->m_seeingValues.minFried_x()*1e2, this->displayFormat, this->displayPrecision));


    this->ui->LE_FriedNow_y->setText(QString::number(this->m_seeingValues.fried_y.last()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMean_y->setText(QString::number(this->m_seeingValues.meanFried_y()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMax_y->setText(QString::number(this->m_seeingValues.maxFried_y()*1e2, this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMin_y->setText(QString::number(this->m_seeingValues.minFried_y()*1e2, this->displayFormat, this->displayPrecision));

    this->ui->LE_SeeingMax->setText(QString::number(this->m_seeingValues.maxSeeing()*1e6, this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingMin->setText(QString::number(this->m_seeingValues.minSeeing()*1e6, this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingNow->setText(QString::number(this->m_seeingValues.seeing.last()*1e6, this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingMean->setText(QString::number(this->m_seeingValues.meanSeeing()*1e6, this->displayFormat, this->displayPrecision));
}
void SeeingGui::initPlots(){
    //set the time format of the x-axis.
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);

    dateTicker->setDateTimeFormat("hh:mm:ss");
    QFont titleFont =  QFont("sans", 12, QFont::Bold);
    QFont legendFont =  QFont("sans", 8, QFont::Bold);
    QFont axisFont = QFont("sans", 12, QFont::Bold);
    QPen xPen;
    QPen yPen;
    xPen.setStyle(Qt::DotLine);
    xPen.setWidthF(2);
    xPen.setColor(Qt::blue);
    yPen.setStyle(Qt::DotLine);
    yPen.setWidthF(2);
    yPen.setColor(Qt::red);

    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    this->ui->FriedPlot->graph(0)->setPen(xPen);
    this->ui->FriedPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    this->ui->FriedPlot->graph(0)->setBrush(QBrush(Qt::blue));
    this->ui->FriedPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    this->ui->FriedPlot->graph(0)->setName(QString("Horizontal Fried parameter"));

    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph(1)->setPen(yPen);
    this->ui->FriedPlot->graph(1)->setBrush(QBrush(Qt::red));
    this->ui->FriedPlot->graph(1)->setLineStyle(QCPGraph::lsNone);
    this->ui->FriedPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    this->ui->FriedPlot->graph(1)->setName(QString("Vertical Fried parameter"));

    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph(2)->setPen(yPen);
    this->avgFriedLabel->setText("Average Fried parameter: ");
    this->avgFriedLabel->setFont(legendFont);
    if (this->ui->FriedPlot->legend->hasElement(3, 0)) // if top cell isn't empty, insert an empty row at top
        this->ui->FriedPlot->legend->insertRow(3);
    this->ui->FriedPlot->legend->addElement(3,0, this->avgFriedLabel);


    this->ui->FriedPlot->xAxis->setLabel("");
    this->ui->FriedPlot->xAxis->setTicker(dateTicker);
    this->ui->FriedPlot->xAxis->setLabelFont(axisFont);
    this->ui->FriedPlot->yAxis->setLabel("[cm]");
    this->ui->FriedPlot->yAxis->setLabelFont(axisFont);

    this->ui->FriedPlot->plotLayout()->insertRow(0);
    this->ui->FriedPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->FriedPlot, "Fried parameter", titleFont));

    this->ui->FriedPlot->legend->setVisible(true);
    this->ui->FriedPlot->legend->setFont(legendFont);
    this->ui->FriedPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    this->ui->FriedPlot->replot();

    this->ui->SeeingPlot->addGraph();
    this->ui->SeeingPlot->graph(0)->setPen(QPen(QColor(255, 110, 40)));
    this->ui->SeeingPlot->graph(0)->setBrush(QBrush(Qt::green));
    this->ui->SeeingPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    this->ui->SeeingPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));


    this->ui->SeeingPlot->xAxis->setLabel("");
    this->ui->SeeingPlot->xAxis->setTicker(dateTicker);
    this->ui->SeeingPlot->xAxis->setLabelFont(axisFont);
    this->ui->SeeingPlot->yAxis->setLabel("[mu rad]");
    this->ui->SeeingPlot->yAxis->setLabelFont(axisFont);

    this->ui->SeeingPlot->plotLayout()->insertRow(0);
    this->ui->SeeingPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->SeeingPlot, "Seeing", titleFont));

    this->ui->SeeingPlot->legend->setVisible(true);
    this->ui->SeeingPlot->legend->setFont(legendFont);
    this->ui->SeeingPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    this->ui->SeeingPlot->replot();
}

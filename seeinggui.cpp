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
    this->ui->SB_WindowingRadius->setMaximum(20);
    this->ui->SB_WindowingRadius->setValue(5);

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

    if (this->measurementType == "DIMM"){
        this->DIMM_thread = std::make_unique<std::thread>(&SeeingGui::DIMM,this); //Initiates a real-time image processing loop while images are captured.
    }
    else if (this->measurementType == "Gaussian"){
        this->displayMessage("No support for Gaussian seeing measurements at this time.");
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
     if(this->DIMM_thread->joinable()){
        this->DIMM_thread->join();
    }
     else {
         this->displayMessage("Error while closing DIMM thread \n", true);
     }
    this->writeResultsToFile();
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
            this->m_seeingValues.fried.push_back(this->getFriedParameter());
            this->m_seeingValues.seeing.push_back(getSeeingFromFried(this->m_seeingValues.fried.last(), this->m_seeingParams.wavelength));
            emit newSeeingValues(); // updates the plots and display in gui
            counter = 1; //reset counter and empty m_DIMMsample
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
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
             this->m_DIMMsample.spotSeparation.push_back(imageprocessing::getSpotSeparation(img, this->m_seeingParams.pxWindowRadius));
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
        this->writeResults << "Timestamp" << "Fried" << "Seeing" << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
        for (int i = 0; i < this->m_seeingValues.fried.size(); i++){
            this->writeResults << this->m_seeingValues.timestamps.at(i) << this->m_seeingValues.fried.at(i) << this->m_seeingValues.seeing.at(i) << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
        }
        this->resultsFile.close();
    }
    else {
        this->displayMessage("Error while creating results file: " + this->directoryPath + "Results.txt", true);
    }
}

double SeeingGui::getFriedParameter(){
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_seeingParams.pxWidth,2)/pow(this->m_seeingParams.focalLength,2);
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_seeingParams.pxHeight,2)/pow(this->m_seeingParams.focalLength,2);
    double fried_x = pow(this->m_seeingParams.K_l*std::pow( angularVariance_x, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.apertureDiameter,-1./3.),3./5.);
    double fried_y = pow(this->m_seeingParams.K_t*std::pow( angularVariance_y, -1)*pow(this->m_seeingParams.wavelength,2)*pow(this->m_seeingParams.apertureDiameter,-1./3.),3./5.);
    return (fried_x + fried_y)/2.0;
}

double SeeingGui::getSeeingFromFried(double FriedParameter, double wavelength){
    return 0.98*wavelength/FriedParameter;
}

void SeeingGui::setMeasurementSettings(){
    this->m_seeingParams.pxWindowRadius = this->m_seeingParams.pxAiryZeros[this->ui->SB_WindowingRadius->value()];
    this->storeImages = this->ui->CheckB_Yes->isChecked();
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
    QCPGraphData dataPoint(this->m_seeingValues.timestamps.last(), this->m_seeingValues.fried.last());
    this->m_seeingValues.friedData.push_back(dataPoint);
    dataPoint.value = getSeeingFromFried(this->m_seeingValues.fried.last(), this->m_seeingParams.wavelength);
    this->m_seeingValues.seeingData.push_back(dataPoint);

    this->ui->FriedPlot->yAxis->setRange(0, this->m_seeingValues.maxFried()*1.5);
    this->ui->FriedPlot->rescaleAxes();
    this->ui->FriedPlot->graph(0)->data()->set(this->m_seeingValues.friedData);

    this->ui->FriedPlot->replot();
    this->ui->FriedPlot->savePdf(this->directoryPath + "FriedPlot.pdf" );

    this->ui->SeeingPlot->yAxis->setRange(0, this->m_seeingValues.maxSeeing()*1.5);
    this->ui->SeeingPlot->rescaleAxes();
    this->ui->SeeingPlot->graph(0)->data()->set(this->m_seeingValues.seeingData);

    this->ui->SeeingPlot->replot();
    this->ui->SeeingPlot->savePdf( this->directoryPath + "SeeingPlot.pdf" );
}

void SeeingGui::updateSeeingPanel(){
    this->ui->LE_FriedMax->setText(QString::number(this->m_seeingValues.maxFried(), this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMin->setText(QString::number(this->m_seeingValues.minFried(), this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedNow->setText(QString::number(this->m_seeingValues.fried.last(), this->displayFormat, this->displayPrecision));
    this->ui->LE_FriedMean->setText(QString::number(this->m_seeingValues.meanFried(), this->displayFormat, this->displayPrecision));

    this->ui->LE_SeeingMax->setText(QString::number(this->m_seeingValues.maxSeeing(), this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingMin->setText(QString::number(this->m_seeingValues.minSeeing(), this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingNow->setText(QString::number(this->m_seeingValues.seeing.last(), this->displayFormat, this->displayPrecision));
    this->ui->LE_SeeingMean->setText(QString::number(this->m_seeingValues.meanSeeing(), this->displayFormat, this->displayPrecision));
}
void SeeingGui::initPlots(){
    //set the time format of the x-axis.
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);

    dateTicker->setDateTimeFormat("hh:mm:ss");
    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    this->ui->FriedPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    this->ui->FriedPlot->xAxis->setLabel("");
    this->ui->FriedPlot->xAxis->setTicker(dateTicker);
    this->ui->FriedPlot->yAxis->setLabel("[cm]");
    this->ui->FriedPlot->plotLayout()->insertRow(0);
    this->ui->FriedPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->FriedPlot, "Fried parameter"));
    this->ui->FriedPlot->replot();

    this->ui->SeeingPlot->addGraph();
    this->ui->SeeingPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 5));
    this->ui->SeeingPlot->graph(0)->setPen(QPen(QColor(255, 110, 40)));
    this->ui->SeeingPlot->xAxis->setLabel("");
    this->ui->SeeingPlot->xAxis->setTicker(dateTicker);
    this->ui->SeeingPlot->yAxis->setLabel("[mu rad]");
    this->ui->SeeingPlot->plotLayout()->insertRow(0);
    this->ui->SeeingPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->SeeingPlot, "Seeing"));
    this->ui->SeeingPlot->replot();
}




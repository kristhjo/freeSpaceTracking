#include "seeinggui.h"
#include "ui_seeinggui.h"

SeeingGui::SeeingGui(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::SeeingGui)
{
    ui->setupUi(this);

    int index = this->ui->ComboB_MeasurementType->findText(this->m_configurationSettings.measurement_type);
    this->ui->ComboB_MeasurementType->setCurrentIndex(index);
    this->m_configurationSettings.measurement_type = this->ui->ComboB_MeasurementType->currentText();

    this->isProcessing = false;
    this->DIMM_thread = nullptr;

    this->ui->CheckB_StoreImages->setChecked(this->m_configurationSettings.store_images);
    this->ui->CheckB_Debug->setChecked(this->m_configurationSettings.debug);

    this->ui->checkB_useThreshold->setChecked(this->m_configurationSettings.use_threshold);
    this->ui->checkB_useWindow->setChecked(this->m_configurationSettings.use_windowing);

    this->ui->LE_SecPerDataPoint->setReadOnly(true);

    this->ui->SB_SampleSize->setValue(this->m_configurationSettings.sample_size);
    this->ui->SB_SampleSize->setMaximum(2000);
    this->ui->SB_WindowingRadius->setMinimum(1);
    this->ui->SB_WindowingRadius->setMaximum(500);
    this->ui->SB_WindowingRadius->setValue(this->m_configurationSettings.window_radius);
    this->ui->SB_airyMin->setMaximum(20);
    this->ui->SB_airyMin->setMinimum(0);
    this->ui->SB_airyMin->setValue(1);

    this->ui->SB_wavelength->setMaximum(1000);

    this->ui->SB_Threshold->setMinimum(1);
    this->ui->SB_Threshold->setMaximum(255);
    this->ui->SB_Threshold->setValue(this->m_configurationSettings.threshold);

    QObject::connect(this->ui->SB_SampleSize,QOverload<int>::of(&QSpinBox::valueChanged),this,&SeeingGui::setSecPerDataPoint,Qt::UniqueConnection);
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(replotSeeing()));
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(updateSeeingPanel()));
    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(debugGaussianSeeing(cv::Mat)), this, SLOT(debugGaussian(cv::Mat)), Qt::UniqueConnection);
    QObject::connect(this, SIGNAL(updateHistogram(int)), this, SLOT(replotHistogram(int)), Qt::UniqueConnection);
    QObject::connect(this->ui->SB_airyMin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SeeingGui::setWindowRadius, Qt::UniqueConnection);

    //QObject::connect(this->ui->SB_WindowingRadius, QOverload<int>::of(&QSpinBox::valueChanged), this, &SeeingGui::setAiryMin, Qt::UniqueConnection);
}

SeeingGui::~SeeingGui(){
    delete ui;
}

/********************************************//**
 *  All signal slot functions
 ***********************************************/
/*void SeeingGui::setAiryMin(){
    if (this->ui->ComboB_MeasurementType->currentText() == "Gaussian fit"){
        this->ui->dSB_airyMin->setValue(double(this->ui->SB_WindowingRadius->value())/this->m_configurationSettings.airy_zeros_pix[0]);
    }
    else if (this->ui->ComboB_MeasurementType->currentText() == "DIMM"){
        this->ui->dSB_airyMin->setValue(double(this->ui->SB_WindowingRadius->value())/this->m_configurationSettings.DIMM_airy_zeros_pix[0]);
    }
}*/
void SeeingGui::setWindowRadius(){


    if (this->ui->ComboB_MeasurementType->currentText() == "Gaussian fit"){
        this->ui->SB_WindowingRadius->setValue(int(this->m_configurationSettings.airy_zeros_pix[this->ui->SB_airyMin->value()] + 0.5));
    }
    else if (this->ui->ComboB_MeasurementType->currentText() == "DIMM"){
        this->ui->SB_WindowingRadius->setValue(int(this->m_configurationSettings.DIMM_airy_zeros_pix[this->ui->SB_airyMin->value()] + 0.5));
    }
}

void SeeingGui::setSecPerDataPoint(){
    this->ui->LE_SecPerDataPoint->setText(QString::number(double(this->ui->SB_SampleSize->value())/this->m_configurationSettings.frameRate, this->displayFormat, 2));
}
void SeeingGui::displayMessage(QString message, bool error){
    this->ui->TE_logSeeing->append(message);
    this->ui->TE_logSeeing->update();
    if (error){
        qDebug() << message;
    }
}
void SeeingGui::configureSettings(){
    int index = this->ui->ComboB_MeasurementType->findText(this->m_configurationSettings.measurement_type);
    this->ui->ComboB_MeasurementType->setCurrentIndex(index);
    this->ui->LE_DirectoryPath->setText(this->m_configurationSettings.storage_location);
    this->ui->CheckB_StoreImages->setChecked(this->m_configurationSettings.store_images);
    this->ui->CheckB_Debug->setChecked(this->m_configurationSettings.debug);
    this->ui->SB_SampleSize->setValue(this->m_configurationSettings.sample_size);
    this->ui->SB_WindowingRadius->setValue(this->m_configurationSettings.window_radius);
    this->ui->SB_Threshold->setValue(this->m_configurationSettings.threshold);
    this->ui->SB_wavelength->setValue(int(this->m_configurationSettings.wavelength*1e9));
    this->ui->SB_wavelength->setEnabled(false);
    this->setSecPerDataPoint();
}
void SeeingGui::startMeasurement(){
    this->setMeasurementSettings();
    this->startOfMeasurement = time(nullptr);
    if(!this->m_configurationSettings.baseDirectory.mkpath(this->m_configurationSettings.storage_location)) {  //Creates a base directory for the measurement where results are stored.
        this->displayMessage("Error creating basedirectory: " + this->m_configurationSettings.storage_location, true);
        return;
    }
    this->createParameterFile();
    this->initPlots();

    this->isMeasuringSeeing->store(true, std::memory_order_release);
    if (this->m_configurationSettings.measurement_type == "DIMM"){
        this->DIMM_thread = std::make_unique<std::thread>(&SeeingGui::DIMM,this); //Initiates a real-time image processing loop while images are captured.
    }
    else if (this->m_configurationSettings.measurement_type == "Gaussian fit"){
        this->displayMessage("No support for Gaussian seeing measurements at this time.");
        this->Gauss_thread = std::make_unique<std::thread>(&SeeingGui::Gaussian,this); //Initiates a real-time image processing loop while images are captured.
        return;
    }
}


void SeeingGui::setMeasurementSettings(){

    this->m_configurationSettings.window_radius = this->ui->SB_WindowingRadius->value();
    this->m_configurationSettings.threshold = this->ui->SB_Threshold->value();

    this->m_configurationSettings.store_images = this->ui->CheckB_StoreImages->isChecked();
    this->m_configurationSettings.debug = this->ui->CheckB_Debug->isChecked();
    this->m_configurationSettings.measurement_type = this->ui->ComboB_MeasurementType->currentText();

    if(this->ui->LE_DirectoryPath->text().back() != "/"){   //corrects for a missing "/" at the end of the directory path in the user input
        this->m_configurationSettings.storage_location = this->ui->LE_DirectoryPath->text() + "/";
    }
    else {
        this->m_configurationSettings.storage_location  = this->ui->LE_DirectoryPath->text();
    }
    this->m_configurationSettings.baseDirectory.setPath(this->m_configurationSettings.storage_location);
    this->m_configurationSettings.sample_size = this->ui->SB_SampleSize->value();
    this->m_imageContainer->sampleSize = this->ui->SB_SampleSize->value();

    //this->m_configurationSettings.wavelength = this->ui->SB_wavelength->value()*1e-9;
}


void SeeingGui::stopMeasurement(){
    this->isMeasuringSeeing->store(false, std::memory_order_release);
    this->endOfMeasurement = time(nullptr);

    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        this->writeParametersToFile << "End of measurement: " << ctime(&this->endOfMeasurement) << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while opening file " + this->m_configurationSettings.storage_location + "SeeingParameters.txt", true);
    }

    while (this->isProcessing.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->displayMessage("Waiting for image processing to finish");
    }
    if (this->m_configurationSettings.measurement_type == "DIMM"){
        if(this->DIMM_thread->joinable()){
            this->DIMM_thread->join();
        }
        else {
            this->displayMessage("Error while closing DIMM thread \n", true);
        }
        this->writeResultsToFile();
    }
    else if(this->m_configurationSettings.measurement_type == "Gaussian fit"){
        if(this->Gauss_thread->joinable()){
            std::cout << "joined gauss thread" << std::endl;
            this->Gauss_thread->join();
        }
        else {
            this->displayMessage("Error while closing Gauss thread \n", true);
        }
        this->writeResultsToFile();
    }
}

void SeeingGui::DIMM(){
    int counter = 1;
    this->isProcessing.store(true, std::memory_order_release);
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){

        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->m_configurationSettings.sample_size){//When counter reaches the sample size, calculate and plot the seeing/fried values
            this->m_seeingValues.fried_x.push_back(this->getFriedParameter_x());
            this->m_seeingValues.fried_y.push_back(this->getFriedParameter_y());
            this->m_seeingValues.fried.push_back(( this->m_seeingValues.fried_x.last() + this->m_seeingValues.fried_y.last() )/2.0);
            this->m_seeingValues.seeing_x.push_back(getSeeingFromFried(this->m_seeingValues.fried_x.last(), this->m_configurationSettings.wavelength));
            this->m_seeingValues.seeing_y.push_back(getSeeingFromFried(this->m_seeingValues.fried_y.last(), this->m_configurationSettings.wavelength));
            this->m_seeingValues.seeing.push_back( (this->m_seeingValues.seeing_x.last() + this->m_seeingValues.seeing_y.last() )/2.0);
            emit newSeeingValues(); // updates the plots and display in gui
            counter = 1; //reset counter and empty m_DIMMsample
            this->m_DIMMsample = {};
        }

        if (counter == 1){//At the beginning of a sample, store the timestamp. If storeImages = true, create a folder where images can be stored.
            this->m_seeingValues.timestamps.push_back(this->m_imageContainer->startTime);
            if(this->m_configurationSettings.store_images){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
        }

        if (!this->m_imageContainer->imgQueue.empty()){ //If there are images queued in the m_imagecontainer, process the oldest one.
            cv::Mat img = this->m_imageContainer->imgQueue.front();
            if (this->m_configurationSettings.store_images){
                if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
            }

            if   (this->m_configurationSettings.debug){
                this->debugDIMM(img, counter);
            }
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
            cv::Point spotDist = imageprocessing::getSpotSeparation(img, this->m_configurationSettings.window_radius);
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
    imageprocessing::cropWindow(img1, croppedImg1, this->m_configurationSettings.window_radius);
    imageprocessing::cropWindow(img2, croppedImg2, this->m_configurationSettings.window_radius);
    cv::Point c1, c2;
    c1 = imageprocessing::findCentroid(croppedImg1);
    c2 = imageprocessing::findCentroid(croppedImg2);
    cv::circle(croppedImg1,c1,1,cv::Scalar(128,0,0),-1); //creates circle on img following the centroid.
    cv::circle(croppedImg2,c2,1,cv::Scalar(128,0,0),-1); //creates circle on img following the centroid.
    cv::hconcat(croppedImg1, croppedImg2, croppedImg1);
    if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/debug_img_" + std::to_string(label) + ".jpg", croppedImg1)){
        emit newMessage("Failed to save debug image", true);
    }
}

void SeeingGui::Gaussian(){
    int counter = 1;
    this->isProcessing.store(true, std::memory_order_release);
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){
        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->m_configurationSettings.sample_size){//When counter reaches the sample size, calculate and plot the seeing/fried values
            if (this->m_configurationSettings.debug){
                emit debugGaussian(this->m_GaussSample.gaussImg);
            }
            this->m_GaussSample.fitParams = imageprocessing::getGaussianFitParams(this->m_GaussSample.gaussImg);

            this->m_seeingValues.seeing_x.push_back(this->m_GaussSample.FWHM_x()*this->m_configurationSettings.pixFOV);
            this->m_seeingValues.seeing_y.push_back(this->m_GaussSample.FWHM_y()*this->m_configurationSettings.pixFOV);

            this->m_seeingValues.fried_x.push_back(this->getFriedFromSeeing(this->m_seeingValues.seeing_x.last(), this->m_configurationSettings.wavelength));
            this->m_seeingValues.fried_y.push_back(this->getFriedFromSeeing(this->m_seeingValues.seeing_y.last(), this->m_configurationSettings.wavelength));
            this->m_seeingValues.fried.push_back(( this->m_seeingValues.fried_x.last() + this->m_seeingValues.fried_y.last() )/2.0);
            this->m_seeingValues.seeing.push_back( (this->m_seeingValues.seeing_x.last() + this->m_seeingValues.seeing_y.last() )/2.0);
            emit newSeeingValues(); // updates the plots and display in gui
            counter = 1; //reset counter and empty m_DIMMsample
            this->m_GaussSample = {};
        }

        if (counter == 1){
            //At the beginning of a sample, store the timestamp.
            //If storeImages = true, create a folder where images can be stored.
            this->m_seeingValues.timestamps.push_back(this->m_imageContainer->startTime);
            if(this->m_configurationSettings.store_images){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
        }

        if (!this->m_imageContainer->imgQueue.empty()){
            //If there are images queued in the m_imagecontainer, process the oldest one.

            cv::Mat img = this->m_imageContainer->imgQueue.front();
            cv::Mat croppedImg;
            if (this->m_configurationSettings.use_threshold){
                imageprocessing::cropThreshold(img, croppedImg, this->m_configurationSettings.threshold);
            }
            else if (this->m_configurationSettings.use_windowing){
                imageprocessing::cropWindow(img, croppedImg, this->m_configurationSettings.window_radius);
            }
            else {
                croppedImg = img;
            }
            this->sumSaturatedPixels[cv::countNonZero(croppedImg == 255)]++;

            //emit updateHistogram(cv::countNonZero(croppedImg == 255));
            croppedImg.convertTo(croppedImg, CV_16UC1);
            /*
            if (cv::countNonZero(croppedImg > 10) < 1){
                std::cout << "skipped image due to low intensity" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            else if (cv::countNonZero(croppedImg == 255) > 10){
                std::cout << "skipped image due to saturated pixels" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            */

            if(counter == 1){
                this->m_GaussSample.gaussImg = croppedImg;
            }
            else {
                cv::add(this->m_GaussSample.gaussImg, croppedImg, this->m_GaussSample.gaussImg, cv::Mat(), CV_16UC1);
            }
            if (this->m_configurationSettings.store_images){
                if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
                if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/gaussImg_" + std::to_string(counter) + ".jpg", this->m_GaussSample.gaussImg)){
                    emit newMessage("Failed to save gaussimg", true);
                }
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

void SeeingGui::replotHistogram(int numSaturatedPixels){
    this->saturatedPixels_hist->addData(double(numSaturatedPixels % 51), 1.0);
    this->ui->saturatedPixelsPlot->rescaleAxes();
    this->ui->saturatedPixelsPlot->replot();
}

void SeeingGui::debugGaussian(cv::Mat image){
    std::cout << "debugging" << std::endl;
    datacontainers::gaussianFitParams fitParams = imageprocessing::getGaussianFitParams(image);
    std::cout << "fitParams: " << "\n";
    std::cout << "center x: " << fitParams.center_x << "\n";
    std::cout << "center y: " << fitParams.center_y << "\n";
    std::cout << "var x: " << fitParams.var_x << "\n";
    std::cout << "var y: " << fitParams.var_y << "\n";
    std::cout << "intensity max: " << fitParams.intensitymax << "\n";
    std::cout << "saturated pixels: " << fitParams.numSaturatedPixels << "\n";
    //std::cout << this->m_GaussSample.gaussImg << std::endl;
    int nx = image.rows;
    int ny = image.cols;
    this->colorMapImage->data()->setSize(nx, ny);
    this->colorMapImage->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
    this->colorMapFit->data()->setSize(nx, ny);
    this->colorMapFit->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
    this->colorMapResidue->data()->setSize(nx, ny);
    this->colorMapResidue->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));

    double x, y, zfit, xIm, yIm, zImage, xRes, yRes, zRes;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        this->colorMapFit->data()->cellToCoord(xIndex, yIndex, &x, &y);
        this->colorMapImage->data()->cellToCoord(xIndex, yIndex, &xIm, &yIm);
        this->colorMapResidue->data()->cellToCoord(xIndex, yIndex, &xRes, &yRes);

        zfit = exp(- (pow((x-fitParams.center_x),2)/(2*fitParams.var_x)) - (pow((y-fitParams.center_y),2)/(2*fitParams.var_y)));
        zImage = image.at<ushort>(cv::Point(xIndex, yIndex))/(fitParams.intensitymax);
        if (zImage > 1){
            std::cout << "uh oh" << std::endl;
            //zImage = 0;
        }
        zRes = abs(zImage-zfit);
        this->colorMapFit->data()->setCell(xIndex, yIndex, zfit);
        this->colorMapImage->data()->setCell(xIndex, yIndex, zImage);
        this->colorMapResidue->data()->setCell(xIndex, yIndex, zRes);
        }
    }
    std::cout << fitParams.intensitymax << " " << image.at<ushort>(cv::Point(fitParams.center_x, fitParams.center_y)) << std::endl;
    this->colorMapImage->rescaleDataRange();
    this->colorMapFit->rescaleDataRange();
    this->colorMapResidue->rescaleDataRange();
    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    this->ui->debugPlot_image->rescaleAxes();
    this->ui->debugPlot_fit->rescaleAxes();
    this->ui->debugPlot_residue->rescaleAxes();

    this->ui->debugPlot_image->replot();
    this->ui->debugPlot_fit->replot();
    this->ui->debugPlot_residue->replot();

    this->saturatedPixels_hist->setData(this->pixelsIndex,this->sumSaturatedPixels);
    this->ui->saturatedPixelsPlot->rescaleAxes();
    this->ui->saturatedPixelsPlot->replot();
    for (int i = 0; i<255 ; i++){
        this->sumSaturatedPixels[i] = 0;
    }

    this->ui->debugPlot_image->savePng(this->m_configurationSettings.storage_location + this->currentSubFolderName + "/debugGaussianImage.png", 400, 400);
    this->ui->debugPlot_fit->savePng(this->m_configurationSettings.storage_location + this->currentSubFolderName + "/debugGaussianFit.png", 400, 400);
    this->ui->debugPlot_residue->savePng(this->m_configurationSettings.storage_location+ this->currentSubFolderName + "/debugGaussianResidue.png", 400, 400);
    std::cout << "finished debugging" << std::endl;


}


void SeeingGui::createParameterFile(){
    this->parameterFile.setFileName(this->m_configurationSettings.storage_location + "SeeingParameters.txt");
    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Text)){
        this->writeParametersToFile.setDevice(&this->parameterFile);
        this->writeParametersToFile << "Seeing Measurement Parameters" << "\n";
        this->writeParametersToFile << "Start of measurement: " << ctime(&this->startOfMeasurement) << "\n";
        this->writeParametersToFile << "Measurement type: " << this->m_configurationSettings.measurement_type << "\n";
        this->writeParametersToFile << "Sample size: " << this->m_configurationSettings.sample_size << "\n";
        this->writeParametersToFile << "Exposure time: " << this->m_configurationSettings.exposureTime << "\n";
        this->writeParametersToFile << "Frame rate: " << this->m_configurationSettings.frameRate << "\n";
        this->writeParametersToFile << "Window radius: " << this->m_configurationSettings.window_radius << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while creating parameter file: " + this->m_configurationSettings.storage_location + "SeeingParameters.txt", true);
    }
}


void SeeingGui::writeResultsToFile(){
    this->resultsFile.setFileName(this->m_configurationSettings.storage_location + "Results.txt");
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
        this->displayMessage("Error while creating results file: " + this->m_configurationSettings.storage_location + "Results.txt", true);
    }
}

double SeeingGui::getFriedParameter(){
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_configurationSettings.pixFOV,2);
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_x = pow(this->m_configurationSettings.K_l*std::pow( angularVariance_x, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    double fried_y = pow(this->m_configurationSettings.K_t*std::pow( angularVariance_y, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return (fried_x + fried_y)/2.0;
}
double SeeingGui::getFriedParameter_x(){
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_x = pow(this->m_configurationSettings.K_l*std::pow( angularVariance_x, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return fried_x;
}
double SeeingGui::getFriedParameter_y(){
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_y = pow(this->m_configurationSettings.K_t*std::pow( angularVariance_y, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return fried_y;
}

double SeeingGui::getSeeingFromFried(double FriedParameter, double wavelength){
    return 0.98*wavelength/FriedParameter;
}
double SeeingGui::getFriedFromSeeing(double SeeingParameter, double wavelength){
    return 0.98*wavelength/SeeingParameter;
}

QString SeeingGui::timestampToFolderName(time_t timestamp){
    QDateTime t(QDateTime::fromTime_t(timestamp));
    return (t.toString(Qt::TextDate));
}




void SeeingGui::replotSeeing(){

    this->m_seeingValues.friedData.push_back(QCPGraphData(this->m_seeingValues.timestamps.last(), this->m_seeingValues.fried.last()*1e2));
    this->m_seeingValues.seeingData.push_back(QCPGraphData(this->m_seeingValues.timestamps.last(), this->m_seeingValues.seeing.last()*1e6));
    this->ui->FriedPlot->graph(0)->data()->set(this->m_seeingValues.friedData);
    this->ui->FriedPlot->yAxis->setRange(0, this->m_seeingValues.maxFried()*1e2*1.2);
    this->ui->FriedPlot->xAxis->rescale();

    this->m_seeingValues.avgFriedPlot.clear();
    this->m_seeingValues.avgFriedPlot.push_back(QCPGraphData(this->m_seeingValues.timestamps.first(), this->m_seeingValues.meanFried()*1e2));
    this->m_seeingValues.avgFriedPlot.push_back(QCPGraphData(this->m_seeingValues.timestamps.last(), this->m_seeingValues.meanFried()*1e2));
    this->ui->FriedPlot->graph(1)->data()->set(this->m_seeingValues.avgFriedPlot);
    this->friedTextLabel->setText("Average Fried parameter: " + QString::number(this->m_seeingValues.meanFried()*1e2, this->displayFormat, this->displayPrecision) + " cm");

    this->ui->FriedPlot->replot();
    this->ui->FriedPlot->savePdf(this->m_configurationSettings.storage_location + "FriedPlot.pdf" );

    this->ui->SeeingPlot->graph(0)->data()->set(this->m_seeingValues.seeingData);

    this->m_seeingValues.avgSeeingPlot.clear();
    this->m_seeingValues.avgSeeingPlot.push_back(QCPGraphData(this->m_seeingValues.timestamps.first(), this->m_seeingValues.meanSeeing()*1e6));
    this->m_seeingValues.avgSeeingPlot.push_back(QCPGraphData(this->m_seeingValues.timestamps.last(), this->m_seeingValues.meanSeeing()*1e6));
    this->ui->SeeingPlot->graph(1)->data()->set(this->m_seeingValues.avgSeeingPlot);
    this->seeingTextLabel->setText("Average seeing: " + QString::number(this->m_seeingValues.meanSeeing()*1e6, this->displayFormat, this->displayPrecision) + " urad");

    this->ui->SeeingPlot->yAxis->setRange(0, this->m_seeingValues.maxSeeing()*1e6*1.2);
    this->ui->SeeingPlot->xAxis->rescale();
    this->ui->SeeingPlot->replot();
    this->ui->SeeingPlot->savePdf( this->m_configurationSettings.storage_location + "SeeingPlot.pdf" );
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
    this->m_seeingValues.friedData.clear();
    this->m_seeingValues.seeingData.clear();
    this->m_seeingValues.timestamps.clear();

    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss");
    QFont titleFont =  QFont("sans", 12, QFont::Bold);
    QFont legendFont =  QFont("sans-serif", 8, QFont::Bold);
    QFont axisFont = QFont("sans", 12, QFont::Bold);
    QPen seeingPen;
    QPen friedPen;
    seeingPen.setStyle(Qt::DotLine);
    seeingPen.setWidthF(2);
    seeingPen.setColor(Qt::red);
    friedPen.setStyle(Qt::DotLine);
    friedPen.setWidthF(2);
    friedPen.setColor(Qt::blue);
    this->ui->FriedPlot->clearGraphs();
    this->ui->SeeingPlot->clearGraphs();
    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    this->ui->FriedPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(Qt::blue), 5));

    this->ui->FriedPlot->addGraph();
    this->ui->FriedPlot->graph(1)->setPen(friedPen);

    this->ui->FriedPlot->xAxis->setLabel("");
    this->ui->FriedPlot->xAxis->setTicker(dateTicker);
    this->ui->FriedPlot->xAxis->setLabelFont(axisFont);
    this->ui->FriedPlot->yAxis->setLabel("[cm]");
    this->ui->FriedPlot->yAxis->setLabelFont(axisFont);

    if(this->firstInitialization){
        this->ui->FriedPlot->plotLayout()->insertRow(0);
        this->ui->FriedPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->FriedPlot, "Fried parameter", titleFont));

        this->friedTextLabel = new QCPItemText(this->ui->FriedPlot);
        this->friedTextLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        this->friedTextLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
        this->friedTextLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
        this->friedTextLabel->setText("Average Fried parameter: xx.xx");
        this->friedTextLabel->setFont(legendFont); // make font a bit larger
        this->friedTextLabel->setPen(QPen(Qt::black)); // show black border around text
        this->friedTextLabel->setBrush(QBrush(QColor(245,245,220, 100)));
        this->friedTextLabel->setPadding(QMargins(3,3,3,3));
    }


    this->ui->FriedPlot->replot();

    this->ui->SeeingPlot->addGraph();
    this->ui->SeeingPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    this->ui->SeeingPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(Qt::green),5));

    this->ui->SeeingPlot->addGraph();
    this->ui->SeeingPlot->graph(1)->setPen(seeingPen);

    this->ui->SeeingPlot->xAxis->setLabel("");
    this->ui->SeeingPlot->xAxis->setTicker(dateTicker);
    this->ui->SeeingPlot->xAxis->setLabelFont(axisFont);
    this->ui->SeeingPlot->yAxis->setLabel("[mu rad]");
    this->ui->SeeingPlot->yAxis->setLabelFont(axisFont);
    if(this->firstInitialization){
        this->ui->SeeingPlot->plotLayout()->insertRow(0);
        this->ui->SeeingPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->SeeingPlot, "Seeing", titleFont));

        this->seeingTextLabel = new QCPItemText(this->ui->SeeingPlot);
        this->seeingTextLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
        this->seeingTextLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
        this->seeingTextLabel->position->setCoords(0.5, 0); // place position at center/top of axis rect
        this->seeingTextLabel->setText("Average seeing: xx.xx");
        this->seeingTextLabel->setFont(legendFont); // make font a bit larger
        this->seeingTextLabel->setPen(QPen(Qt::black)); // show black border around text
        this->seeingTextLabel->setBrush(QBrush(QColor(245,245,220, 200)));
        this->seeingTextLabel->setPadding(QMargins(3,3,3,3));
    }

    this->ui->SeeingPlot->replot();

    if (this->m_configurationSettings.debug){
        if (this->m_configurationSettings.measurement_type == "Gaussian fit"){

            this->colorMapImage = new QCPColorMap(this->ui->debugPlot_image->xAxis, this->ui->debugPlot_image->yAxis);
            this->colorMapFit = new QCPColorMap(this->ui->debugPlot_fit->xAxis, this->ui->debugPlot_fit->yAxis);
            this->colorMapResidue= new QCPColorMap(this->ui->debugPlot_residue->xAxis, this->ui->debugPlot_residue->yAxis);
            this->ui->debugPlot_image->axisRect()->setupFullAxesBox(true);
            this->ui->debugPlot_image->xAxis->setLabel("x");
            this->ui->debugPlot_image->yAxis->setLabel("y");
            this->ui->debugPlot_fit->axisRect()->setupFullAxesBox(true);
            this->ui->debugPlot_fit->xAxis->setLabel("x");
            this->ui->debugPlot_fit->yAxis->setLabel("y");
            this->ui->debugPlot_residue->axisRect()->setupFullAxesBox(true);
            this->ui->debugPlot_residue->xAxis->setLabel("x");
            this->ui->debugPlot_residue->yAxis->setLabel("y");
            this->colorScaleImage = new QCPColorScale(this->ui->debugPlot_image);
            this->colorScaleFit = new QCPColorScale(this->ui->debugPlot_fit);
            this->colorScaleResidue = new QCPColorScale(this->ui->debugPlot_residue);
            this->ui->debugPlot_image->plotLayout()->addElement(0, 1, colorScaleImage); // add it to the left of the main axis rect
            this->ui->debugPlot_fit->plotLayout()->addElement(0, 1, colorScaleFit); // add it to the right of the main axis rect
            this->ui->debugPlot_residue->plotLayout()->addElement(0,1, colorScaleResidue);



            colorScaleImage->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
            colorMapImage->setColorScale(colorScaleImage); // associate the color map with the color scale
            colorScaleFit->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
            colorMapFit->setColorScale(colorScaleFit); // associate the color map with the color scale
            colorScaleResidue->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
            colorMapResidue->setColorScale(colorScaleResidue); // associate the color map with the color scale

            colorMapImage->setGradient(QCPColorGradient::gpPolar);
            colorMapFit->setGradient(QCPColorGradient::gpPolar);
            colorMapResidue->setGradient(QCPColorGradient::gpPolar);


            colorMapImage->setDataRange(QCPRange(0,1));
            colorMapFit->setDataRange(QCPRange(0,1));
            colorMapResidue->setDataRange(QCPRange(0,1));
            // we could have also created a QCPColorGradient instance and added own colors to
            // the gradient, see the documentation of QCPColorGradient for what's possible.

            // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
            colorMapImage->rescaleDataRange();
            colorMapFit->rescaleDataRange();
            colorMapResidue->rescaleDataRange();
            // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
            QCPMarginGroup *marginGroupImage = new QCPMarginGroup(this->ui->debugPlot_image);
            this->ui->debugPlot_image->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupImage);
            colorScaleImage->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupImage);

            QCPMarginGroup *marginGroupFit = new QCPMarginGroup(this->ui->debugPlot_fit);
            this->ui->debugPlot_fit->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupFit);
            colorScaleFit->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupFit);

            QCPMarginGroup *marginGroupResidue= new QCPMarginGroup(this->ui->debugPlot_residue);
            this->ui->debugPlot_residue->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupResidue);
            colorScaleResidue->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroupResidue);

            if(this->firstInitialization){
                this->ui->debugPlot_fit->plotLayout()->insertRow(0);
                this->ui->debugPlot_image->plotLayout()->insertRow(0);
                this->ui->debugPlot_residue->plotLayout()->insertRow(0);
                this->ui->debugPlot_fit->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->debugPlot_fit, "Gaussian fit", titleFont));
                this->ui->debugPlot_image->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->debugPlot_image, "Image", titleFont));
                this->ui->debugPlot_residue->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->debugPlot_residue, "Residue", titleFont));

            }

            this->ui->debugPlot_image->rescaleAxes();
            this->ui->debugPlot_image->replot();
            this->ui->debugPlot_fit->rescaleAxes();
            this->ui->debugPlot_fit->replot();
            this->ui->debugPlot_residue->rescaleAxes();
            this->ui->debugPlot_residue->replot();


            this->saturatedPixels_hist = new QCPBars(this->ui->saturatedPixelsPlot->xAxis, this->ui->saturatedPixelsPlot->yAxis);
            this->saturatedPixels_hist->setName("Saturated pixels");
            if(this->firstInitialization){
                for (int i =0; i<255; i++){
                this->pixelsIndex.append(i);
                this->sumSaturatedPixels.append(0);
                }
            }

            this->ui->saturatedPixelsPlot->rescaleAxes();
            this->ui->saturatedPixelsPlot->replot();

        }
    }
    this->firstInitialization = false;
}

 #include "seeinggui.h"
#include "ui_seeinggui.h"

SeeingGui::SeeingGui(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::SeeingGui)
{
    ui->setupUi(this);
    this->measurement_thread = nullptr;
    this->isProcessing = false;
    QObject::connect(this->ui->SB_SampleSize,QOverload<int>::of(&QSpinBox::valueChanged),this,&SeeingGui::setSecPerDataPoint,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_airyMin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SeeingGui::setWindowRadius, Qt::UniqueConnection);
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(updateScatterPlots()));
    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(debugGaussianSeeing(cv::Mat)), this, SLOT(debugGaussian(cv::Mat)), Qt::UniqueConnection);
}

SeeingGui::~SeeingGui(){
    std::cout << "seeingGui destructor called"<< std::endl;
    delete ui;
    std::cout << "succesfully destroyed seeingGui"<< std::endl;
}

void SeeingGui::setWindowRadius(){
    if (this->ui->ComboB_MeasurementType->currentText() == "Gaussian fit"){
        this->ui->SB_WindowingRadius->setValue(int(this->m_configurationSettings.airy_zeros_pix[this->ui->SB_airyMin->value()] + 0.5));
    }
    else if (this->ui->ComboB_MeasurementType->currentText() == "DIMM"){
        this->ui->SB_WindowingRadius->setValue(int(this->m_configurationSettings.DIMM_airy_zeros_pix[this->ui->SB_airyMin->value()] + 0.5));
    }
}
void SeeingGui::setSecPerDataPoint(){
    this->ui->LE_SecPerDataPoint->setText(QString::number(double(this->ui->SB_SampleSize->value())/this->m_configurationSettings.frameRate, 'f', 2));
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
    this->ui->SB_Threshold->setMinimum(1);
    this->ui->SB_Threshold->setMaximum(255);
    this->ui->SB_Threshold->setValue(this->m_configurationSettings.threshold);
    this->ui->LE_DirectoryPath->setText(this->m_configurationSettings.storage_location);
    this->ui->SB_SampleSize->setValue(this->m_configurationSettings.sample_size);
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
    this->m_seeingData = seeingData(this->m_configurationSettings.measurement_type, this->m_configurationSettings.debug);
    this->plotsWindow.release();
    this->setup_plots_window();
    this->isMeasuringSeeing->store(true, std::memory_order_release);
    if (this->m_configurationSettings.measurement_type == "DIMM"){
        this->measurement_thread = std::make_unique<std::thread>(&SeeingGui::DIMM,this);
        return;
    }
    else if (this->m_configurationSettings.measurement_type == "Gaussian fit"){
        this->displayMessage("No support for Gaussian seeing measurements at this time.");
        this->measurement_thread = std::make_unique<std::thread>(&SeeingGui::Gaussian,this);
        return;
    }
}

void SeeingGui::setup_plots_window(){
    this->plotsWindow = std::make_unique<plotDisplay>();
    this->plotsWindow->add_plot(this->plots.seeing);

    this->plotsWindow->add_plot(this->plots.fried);
    this->plots.seeing.add_plottable(scatterPlotConfig(this->m_seeingData.seeing));
    //this->plots.seeing.add_plottable(scatterPlotConfig(this->m_seeingData.avgSeeing, "", QCPGraph::lsLine));
    this->plots.seeing.add_itembox();
    this->plots.fried.add_plottable(scatterPlotConfig(this->m_seeingData.fried));
    //this->plots.fried.add_plottable(scatterPlotConfig(this->m_seeingData.avgFried, "", QCPGraph::lsLine));
    this->plots.fried.add_itembox();

    if(this->m_configurationSettings.debug){
        this->plotsWindow->add_plot(this->plots.centroid);
        this->plotsWindow->add_plot(this->plots.saturation);
        this->plotsWindow->add_plot(this->plots.cm_image);
        this->plotsWindow->add_plot(this->plots.cm_fit);
        this->plotsWindow->add_plot(this->plots.cm_residue);
        this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_x, "X", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5)));
        this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_y, "Y", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 5)));
        this->plots.saturation.add_plottable(histogramConfig(this->m_seeingData.saturation));
        //this->plots.cm_image.add_plottable(colormapConfig(this->m_seeingData.cm_image));
        //this->plots.cm_fit.add_plottable(colormapConfig(this->m_seeingData.cm_fit));
        //this->plots.cm_residue.add_plottable(colormapConfig(this->m_seeingData.cm_residue));
        this->plots.cm_image.add_plottable(colormapConfig());
        this->plots.cm_fit.add_plottable(colormapConfig());
        this->plots.cm_residue.add_plottable(colormapConfig());


    }
    this->plotsWindow->show();
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
}


void SeeingGui::stopMeasurement(){
    this->isMeasuringSeeing->store(false, std::memory_order_release);
    //this->endOfMeasurement = time(nullptr);

    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        this->writeParametersToFile << "End of measurement: " << ctime(nullptr) << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while opening file " + this->m_configurationSettings.storage_location + "SeeingParameters.txt", true);
    }
    while (this->isProcessing.load(std::memory_order_acquire)) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        this->displayMessage("Waiting for image processing to finish");
    }
    if(this->measurement_thread->joinable()){
        std::cout << "joining with measurement thread" << std::endl;
        this->measurement_thread->join();
    }
    else {
        this->displayMessage("Error while closing measurement thread \n", true);
    }
    this->writeResultsToFile();

    //this->m_seeingData = {};
}

void SeeingGui::DIMM(){
    int counter = 1;
    time_t timestamp = time(nullptr);
    this->isProcessing.store(true, std::memory_order_release);
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){

        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->m_configurationSettings.sample_size){//When counter reaches the sample size, calculate and plot the seeing/fried values
            /*
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
            timestamp = this->m_imageContainer->startTime;
            if(this->m_configurationSettings.store_images){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
        */
        }

        if (!this->m_imageContainer->imgQueue.empty()){ //If there are images queued in the m_imagecontainer, process the oldest one.
            cv::Mat img = this->m_imageContainer->imgQueue.front();
            if (this->m_configurationSettings.store_images){
                if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
            }

            if   (this->m_configurationSettings.debug){
                this->debugDIMM(img);
            }
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
            cv::Point spotDist = imageprocessing::getSpotSeparation(img, this->m_configurationSettings.window_radius);
            /*
             * //if strehl less than cut off
            */
            this->m_DIMMsample.spotSeparation.push_back(spotDist);
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

void SeeingGui::debugDIMM(const cv::Mat& image){
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
    if(!cv::imwrite( this->m_configurationSettings.storage_location.toStdString() + this->currentSubFolderName.toStdString()  + "/debug_img_" + ".jpg", croppedImg1)){
        emit newMessage("Failed to save debug image", true);
    }
}

void SeeingGui::Gaussian(){
    int counter = 1;
    this->m_GaussSample = {};
    QCPGraphData seeing;
    QCPGraphData fried;
    time_t timestamp = time(nullptr);
    double strehl_ratio;
    this->isProcessing.store(true, std::memory_order_release);

    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){
        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->m_configurationSettings.sample_size){//When counter reaches the sample size, calculate and plot the seeing/fried values
            if (this->m_configurationSettings.debug){
                emit debugGaussian(this->m_GaussSample.gaussImg.clone());
                cv::Point centroid = imageprocessing::findCentroid(this->m_GaussSample.gaussImg.clone());
                this->m_seeingData.centroid_x->add(QCPGraphData(timestamp,centroid.x));
                this->m_seeingData.centroid_y->add(QCPGraphData(timestamp,centroid.y));                
            }

            strehl_ratio = imageprocessing::getStrehlRatio(this->m_GaussSample.gaussImg.clone(), this->m_configurationSettings.pixel_size,this->m_configurationSettings.wavelength, this->m_configurationSettings.apertureDiameter);
            std::cout << "strehl " << strehl_ratio << std::endl;
            this->m_GaussSample.fitParams = imageprocessing::getGaussianFitParams(this->m_GaussSample.gaussImg.clone());
            if (!this->m_GaussSample.fitParams.valid){
                emit newMessage("fit params not valid ", true);
                counter = 1; //reset counter and empty m_GaussSample
                this->m_GaussSample = {};
                continue;
            }
            seeing.value = 0.5*(this->m_GaussSample.FWHM_x()*this->m_configurationSettings.pixFOV + this->m_GaussSample.FWHM_y()*this->m_configurationSettings.pixFOV)*1e6;
            fried.value = this->getFriedFromSeeing(seeing.value, this->m_configurationSettings.wavelength);
            this->m_seeingData.seeing->add(QCPGraphData(timestamp, seeing.value));
            this->m_seeingData.fried->add(QCPGraphData(timestamp, this->getFriedFromSeeing(seeing.value, this->m_configurationSettings.wavelength)));

            //this->m_seeingData.strehlData_Gauss->add(QCPGraphData(timestamp, strehl_ratio));
            emit newSeeingValues();
            counter = 1; //reset counter and empty m_GaussSample
            this->m_GaussSample = {};
        }

        if (counter == 1){
            //At the beginning of a sample, store the timestamp.
            //If storeImages = true, create a folder where images can be stored.
            timestamp = this->m_imageContainer->startTime;
            if(this->m_configurationSettings.store_images){
                this->currentSubFolderName = this->timestampToFolderName(timestamp);
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentSubFolderName, true );
                }
            }
            if(this->m_configurationSettings.debug){
                for(int i = 0; i< 256; i++){
                    (this->m_seeingData.saturation.data()->begin()+i)->value = 0.0;
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
            if (this->m_configurationSettings.debug){
                cv::Mat hist;
                imageprocessing::computeHistogram(croppedImg.clone(), hist);

                for(int i = 0; i < hist.rows; i++){
                    (this->m_seeingData.saturation.data()->begin()+i)->value += hist.at<float>(i);
                }
            }

            if (cv::countNonZero(croppedImg == 255) > this->m_configurationSettings.pixel_saturation_cutOff){
                std::cout << "skipped image due to saturated pixels" << std::endl;
                this->m_imageContainer->removeFirstImg();
                continue;
            }
            croppedImg.convertTo(croppedImg, CV_16UC1);
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

void SeeingGui::debugGaussian(const cv::Mat image){
    std::cout << "debugging gaussian fit" << std::endl;
    datacontainers::gaussianFitParams fitParams = imageprocessing::getGaussianFitParams(image);
    if (!fitParams.valid){
        emit newMessage("image not valid for gaussian fit", true);
        return;
    }
    int nx = image.cols;
    int ny = image.rows;
    this->plots.cm_image.plottable.colormap->data()->setSize(nx, ny);
    this->plots.cm_image.plottable.colormap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
    this->plots.cm_fit.plottable.colormap->data()->setSize(nx, ny);
    this->plots.cm_fit.plottable.colormap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
    this->plots.cm_residue.plottable.colormap->data()->setSize(nx, ny);
    this->plots.cm_residue.plottable.colormap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));

    double x, y, zfit, xIm, yIm, zImage, xRes, yRes, zRes;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        this->plots.cm_image.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &xIm, &yIm);
        this->plots.cm_fit.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        this->plots.cm_residue.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &xRes, &yRes);

        zfit = exp(- (pow((x-fitParams.center_x),2)/(2*fitParams.var_x)) - (pow((y-fitParams.center_y),2)/(2*fitParams.var_y)));
        zImage = image.at<ushort>(cv::Point(xIndex, yIndex))/(fitParams.intensitymax);
        zRes = abs(zImage-zfit);
        if (zImage > 1){
            //std::cout << "uh oh" << std::endl;
            //zImage = 0;
        }
        this->plots.cm_image.plottable.colormap->data()->setCell(xIndex, yIndex, zImage);
        this->plots.cm_fit.plottable.colormap->data()->setCell(xIndex, yIndex, zfit);
        this->plots.cm_residue.plottable.colormap->data()->setCell(xIndex, yIndex, zRes);
        }
    }
    this->plots.cm_image.replot();
    this->plots.cm_fit.replot();
    this->plots.cm_residue.replot();
}

void SeeingGui::updateScatterPlots(){
    this->plots.fried.replot();
    this->plots.seeing.replot();
    this->plots.fried.item->setText("Average Fried parameter: " + QString::number(this->getAverage(*this->m_seeingData.fried), 'f', 3) + " cm");
    this->plots.seeing.item->setText("Average seeing: " + QString::number(this->getAverage(*this->m_seeingData.seeing), 'f', 3) + " urad");

    if (this->m_configurationSettings.debug){
        this->plots.centroid.replot();
        this->plots.saturation.replot();
    }
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
        this->writeParametersToFile << "Wavelength: " << this->m_configurationSettings.wavelength << "\n";
        if (this->m_configurationSettings.use_windowing){
            this->writeParametersToFile << "Window radius: " << this->m_configurationSettings.window_radius << "\n";
        }
        if (this->m_configurationSettings.use_threshold){
            this->writeParametersToFile << "Threshold: " << this->m_configurationSettings.threshold << "\n";
        }
        if(this->m_configurationSettings.measurement_type == "DIMM"){
            this->writeParametersToFile << "Strehl ratio cut-off: " << this->m_configurationSettings.strehl_cutOff << "\n";
        }
        this->writeParametersToFile << "pixel saturation cut-off: " << this->m_configurationSettings.pixel_saturation_cutOff << "\n";

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
        if (this->m_configurationSettings.debug){
            this->writeResults << "Timestamp" << "Fried" << "Seeing" << "Centroid x" << "Centroid y" << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
            for (int i = 0; i < this->m_seeingData.fried->size(); i++){
                this->writeResults << this->m_seeingData.fried->at(i)->key << this->m_seeingData.fried->at(i)->value << this->m_seeingData.seeing->at(i)->value << this->m_seeingData.centroid_x->at(i)->value << this->m_seeingData.centroid_y->at(i)->value << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
            }
        }
        else{
            this->writeResults << "Timestamp" << "Fried" << "Seeing" << qSetFieldWidth(0) << endl << qSetFieldWidth(20);
            for (int i = 0; i < this->m_seeingData.fried->size(); i++){
                this->writeResults << this->m_seeingData.fried->at(i)->key << this->m_seeingData.fried->at(i)->value << this->m_seeingData.seeing->at(i)->value <<qSetFieldWidth(0) << endl << qSetFieldWidth(20);
            }
        }
        this->resultsFile.close();
    }
    else {
        this->displayMessage("Error while creating results file: " + this->m_configurationSettings.storage_location + "Results.txt", true);
    }
}

double SeeingGui::getFriedParameter(){
    ///return fried parameter in units of cm
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_configurationSettings.pixFOV,2);
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_x = pow(this->m_configurationSettings.K_l*std::pow( angularVariance_x, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    double fried_y = pow(this->m_configurationSettings.K_t*std::pow( angularVariance_y, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return (fried_x + fried_y)*1e2/2.0;
}
double SeeingGui::getFriedParameter_x(){
    ///return fried parameter in units of cm
    double angularVariance_x = this->m_DIMMsample.variance_x()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_x = pow(this->m_configurationSettings.K_l*std::pow( angularVariance_x, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return fried_x*1e2;
}
double SeeingGui::getFriedParameter_y(){
    ///return fried parameter in units of cm
    double angularVariance_y = this->m_DIMMsample.variance_y()*pow(this->m_configurationSettings.pixFOV,2);
    double fried_y = pow(this->m_configurationSettings.K_t*std::pow( angularVariance_y, -1)*pow(this->m_configurationSettings.wavelength,2)*pow(this->m_configurationSettings.DIMM_apertureDiameter,-1./3.),3./5.);
    return fried_y*1e2;
}

double SeeingGui::getSeeingFromFried(double FriedParameter, double wavelength){
    ///assumes fried parameter in units of cm, return seeing in units of urad.
    return 0.98*wavelength*1e8/FriedParameter;
}
double SeeingGui::getFriedFromSeeing(double SeeingParameter, double wavelength){
    ///assumes seeing in units of urad, returns fried in units of cm.
    return 0.98*wavelength*1e8/SeeingParameter;
}

double SeeingGui::getAverage(QCPGraphDataContainer graph){
    double sum = 0.0;
    double num = 0.0;
    for (auto it = graph.begin(); it!=graph.end(); it++){
         sum+= it->value;
         num++;
    }
    return sum/num;
}

QString SeeingGui::timestampToFolderName(time_t timestamp){
   return QDateTime::fromTime_t(static_cast<uint>(timestamp)).toString(Qt::TextDate);
}


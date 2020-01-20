#include "seeinggui.h"
#include "ui_seeinggui.h"

SeeingGui::SeeingGui(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::SeeingGui)
{
    ui->setupUi(this);
    this->measurement_thread = nullptr;
    this->isProcessing = false;
    QDir dir;
    dir.cdUp();
    this->projectFolderPath = dir.path();
    QObject::connect(this->ui->SB_SampleSize,QOverload<int>::of(&QSpinBox::valueChanged),this,&SeeingGui::setSecPerDataPoint,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_airyMin, QOverload<int>::of(&QSpinBox::valueChanged), this, &SeeingGui::setWindowRadius, Qt::UniqueConnection);
    QObject::connect(this, SIGNAL(newSeeingValues()), this, SLOT(updateGaussPlots()));
    QObject::connect(this, SIGNAL(newDIMMValues()), this, SLOT(updateDIMMPlots()));


    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(debugGaussianSeeing(cv::Mat)), this, SLOT(debugGaussian(cv::Mat)), Qt::UniqueConnection);

}

SeeingGui::~SeeingGui(){
    std::cout << "seeingGui destructor called"<< std::endl;
    delete ui;
    std::cout << "succesfully destroyed seeingGui"<< std::endl;
}

void SeeingGui::loadConfiguration(QString configurationFilePath){
    QSettings settings(configurationFilePath, QSettings::IniFormat);
    settings.beginGroup("SeeingGui");
    this->m_configurationSettings.sample_size = settings.value("sample_size", 50).toInt();
    this->m_configurationSettings.store_images = settings.value("store_images", false).toBool();
    this->m_configurationSettings.debug = settings.value("debug", false).toBool();
    this->m_configurationSettings.use_threshold = settings.value("use_threshold", false).toBool();
    this->m_configurationSettings.use_windowing = settings.value("use_windowing", false).toBool();
    this->m_configurationSettings.window_radius = settings.value("window_radius", 1).toInt();
    this->m_configurationSettings.threshold = settings.value("threshold", 1).toInt();
    this->m_configurationSettings.storage_location = settings.value("storage_location", this->projectFolderPath+"/results/seeing/").toString();
    this->m_configurationSettings.measurement_type = settings.value("measurement_type", "Gaussian fit").toString();
    this->m_configurationSettings.wavelength = settings.value("wavelength", 532e-9).toDouble();
    this->m_configurationSettings.pixel_saturation_cutOff = settings.value("saturation_cutOff",100).toInt();
    settings.endGroup();
    int errorValue = 0;
    settings.beginGroup("SeeingOptics");
    this->m_configurationSettings.magnification_telescope = settings.value("magnification_telescope", errorValue).toDouble();
    this->m_configurationSettings.apertureDiameter = settings.value("aperture_diameter", errorValue).toDouble();
    this->m_configurationSettings.focalLength_eyepiece = settings.value("focalLength_eyepiece", errorValue).toDouble();
    this->m_configurationSettings.pixel_size = settings.value("pixel_size",errorValue).toDouble();
    if(this->m_configurationSettings.magnification_telescope == errorValue || this->m_configurationSettings.pixel_size == errorValue || this->m_configurationSettings.focalLength_eyepiece == errorValue || this->m_configurationSettings.apertureDiameter == errorValue){
        this->newMessage("Warning: it seems like the settings of the SeeingOptics was not set correctly in the .ini file. This will cause errors later.", true);
    }

    this->m_configurationSettings.DIMM_baseline = settings.value("DIMM_baseline", 1.0).toDouble();
    this->m_configurationSettings.DIMM_apertureDiameter = settings.value("DIMM_apertureDiameter", 1.0).toDouble();
    this->m_configurationSettings.update_DIMM_coefficients();
    this->m_configurationSettings.update_airy_zeros();
    this->m_configurationSettings.update_pix_FoV();
    configureSettings();
}

void SeeingGui::setCameraSettings(int frameRate, int exposureTime){
    this->m_configurationSettings.frameRate = frameRate;
    this->m_configurationSettings.exposureTime = exposureTime;
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
    this->ui->SB_saturationCutOff->setValue(this->m_configurationSettings.pixel_saturation_cutOff);
    this->setSecPerDataPoint();
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

void SeeingGui::startMeasurement(){
    this->setMeasurementSettings();
    this->startOfMeasurement = time(nullptr);
    this->m_configurationSettings.measurement_folder_name = "Measurement " + this->timestampToFolderName(startOfMeasurement) + "/";
    this->currentFolderPath = this->m_configurationSettings.storage_location + this->m_configurationSettings.measurement_folder_name;
    if(!this->m_configurationSettings.baseDirectory.mkpath(this->currentFolderPath)) {  //Creates a base directory for the measurement where results are stored.
        this->displayMessage("Error creating basedirectory: " + this->currentFolderPath, true);
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
    savingSettings stdSetting("", this->currentFolderPath, true, "pdf", 1000, 600);
    savingSettings debugSettings("", this->currentFolderPath, this->m_configurationSettings.store_images, "pdf", 800, 400 );
    this->plots.seeing.save_settings = stdSetting;
    this->plotsWindow->add_plot(this->plots.seeing);
    this->plots.fried.save_settings = stdSetting;
    this->plotsWindow->add_plot(this->plots.fried);
    this->plots.seeing.add_plottable(scatterPlotConfig(this->m_seeingData.seeing));
    this->plots.seeing.add_itembox();
    this->plots.fried.add_plottable(scatterPlotConfig(this->m_seeingData.fried));
    this->plots.fried.add_itembox();

    if(this->m_configurationSettings.debug){
        this->plots.centroid.save_settings = stdSetting;
        this->plots.saturation.save_settings  = debugSettings;
        this->plotsWindow->add_plot(this->plots.centroid);
        this->plotsWindow->add_plot(this->plots.saturation);
        this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_x, "X", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5)));
        this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_y, "Y", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 5)));

        this->plots.saturation.add_plottable(histogramConfig(this->m_seeingData.saturation));
        if(this->m_configurationSettings.measurement_type == "Gaussian fit"){
            this->plots.cm_fit.save_settings = debugSettings;
            this->plots.cm_residue.save_settings = debugSettings;
            this->plots.cm_image.save_settings = debugSettings;
            this->plotsWindow->add_plot(this->plots.cm_image);
            this->plotsWindow->add_plot(this->plots.cm_fit);
            this->plotsWindow->add_plot(this->plots.cm_residue);
            this->plots.cm_image.add_plottable(colormapConfig());
            this->plots.cm_fit.add_plottable(colormapConfig());
            this->plots.cm_residue.add_plottable(colormapConfig());
            this->plotsWindow->add_plot(this->plots.gaussOverlap_x);
            this->plots.gaussOverlap_x.add_plottable(scatterPlotConfig(this->m_seeingData.crossSectionX, "fit", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5), QPen(QColor(255,140,0)), QCPAxis::atLeft, false));
            this->plots.gaussOverlap_x.add_plottable(scatterPlotConfig(this->m_seeingData.crossSectionImageX, "image", QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 5), QPen(QColor(10, 140, 70)), QCPAxis::atLeft, false));
            this->plotsWindow->add_plot(this->plots.gaussOverlap_y);
            this->plots.gaussOverlap_y.add_plottable(scatterPlotConfig(this->m_seeingData.crossSectionY, "fit", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5), QPen(QColor(255,140,0)), QCPAxis::atLeft, false));
            this->plots.gaussOverlap_y.add_plottable(scatterPlotConfig(this->m_seeingData.crossSectionImageY, "image", QCPGraph::lsLine,QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 5), QPen(QColor(10, 140, 70)), QCPAxis::atLeft, false));
        }
        if(this->m_configurationSettings.measurement_type == "DIMM"){
            this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_x2, "X2", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 5)));
            this->plots.centroid.add_plottable(scatterPlotConfig(this->m_seeingData.centroid_y2, "Y2", QCPGraph::lsNone, QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 5)));

            this->plots.strehl.save_settings = debugSettings;
            this->plots.vVar.save_settings = debugSettings;
            this->plots.hVar.save_settings = debugSettings;
            this->plotsWindow->add_plot(this->plots.strehl);
            this->plotsWindow->add_plot(this->plots.vVar);
            this->plotsWindow->add_plot(this->plots.hVar);
            this->plots.strehl.add_plottable(histogramConfig(this->m_seeingData.strehlData_DIMM1));
            this->plots.strehl.add_plottable(histogramConfig(this->m_seeingData.strehlData_DIMM2));
            this->plots.vVar.add_plottable(histogramConfig(this->m_seeingData.verticalVarData));
            this->plots.hVar.add_plottable(histogramConfig(this->m_seeingData.horizontalVarData));
        }
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
    this->m_configurationSettings.pixel_saturation_cutOff = this->ui->SB_saturationCutOff->value();
    this->m_configurationSettings.use_threshold = this->ui->checkB_useThreshold->isChecked();
    this->m_configurationSettings.use_windowing = this->ui->checkB_useWindow->isChecked();
    this->m_configurationSettings.threshold = this->ui->SB_Threshold->value();
    this->m_configurationSettings.window_radius = this->ui->SB_WindowingRadius->value();
}


void SeeingGui::stopMeasurement(){
    this->isMeasuringSeeing->store(false, std::memory_order_release);
    //this->endOfMeasurement = time(nullptr);

    if(this->parameterFile.open(QIODevice::WriteOnly | QIODevice::Append)){
        this->writeParametersToFile << "End of measurement: " << ctime(nullptr) << "\n";
        this->parameterFile.close();
    }
    else {
        this->displayMessage("Error while opening file " + this->currentFolderPath + "SeeingParameters.txt", true);
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
    QCPGraphData seeing;
    QCPGraphData fried;
    cv::Point avgCentroid1(0,0);
    cv::Point avgCentroid2(0,0);
    this->isProcessing.store(true, std::memory_order_release);
    while ((this->isMeasuringSeeing->load(std::memory_order_acquire)==true) || (this->m_imageContainer->imgQueue.size() != 0)){

        if(this->m_imageContainer->imgQueue.size() == 0){
            emit newMessage("waiting for imgQueue to fill up"); //DIMM() is not run through the main SeeingGui thread, so interaction with the gui such as text display must be implemented through signals/slots.
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
        if (counter == this->m_configurationSettings.sample_size){//When counter reaches the sample size, calculate and plot the seeing/fried values
            seeing.key = timestamp;
            fried.key = timestamp;
            fried.value = this->getFriedParameter();
            seeing.value = this->getSeeingFromFried(fried.value, this->m_configurationSettings.wavelength);

            this->m_seeingData.fried->add(fried);
            this->m_seeingData.seeing->add(seeing);
            if(this->m_configurationSettings.debug){
                this->m_seeingData.centroid_x->add(QCPGraphData(timestamp, this->m_DIMMsample.avgCentroid1.x/this->m_configurationSettings.sample_size));
                this->m_seeingData.centroid_y->add(QCPGraphData(timestamp, this->m_DIMMsample.avgCentroid1.y/this->m_configurationSettings.sample_size));
                this->m_seeingData.centroid_x2->add(QCPGraphData(timestamp, this->m_DIMMsample.avgCentroid2.x/this->m_configurationSettings.sample_size));
                this->m_seeingData.centroid_y2->add(QCPGraphData(timestamp, this->m_DIMMsample.avgCentroid2.y/this->m_configurationSettings.sample_size));
                std::sort(this->m_DIMMsample.xSeparation.begin(), this->m_DIMMsample.xSeparation.end());
                std::sort(this->m_DIMMsample.ySeparation.begin(), this->m_DIMMsample.ySeparation.end());
                std::sort(this->m_DIMMsample.strehl1.begin(), this->m_DIMMsample.strehl1.end());
                std::sort(this->m_DIMMsample.strehl2.begin(), this->m_DIMMsample.strehl2.end());
                int xcount =0, ycount = 0, strehlcount1 = 0, strehlcount2=0;
                int lastX = this->m_DIMMsample.xSeparation.at(0);
                int lastY = this->m_DIMMsample.ySeparation.at(0);
                double lastStrehl1 = this->m_DIMMsample.strehl1.at(0);
                double lastStrehl2 = this->m_DIMMsample.strehl2.at(0);
                QCPBarsDataContainer xVar, yVar, strehl_distribution1, strehl_distribution2;
                for(size_t i = 0; i < this->m_DIMMsample.xSeparation.size();i++){
                    if(this->m_DIMMsample.xSeparation.at(i) == lastX){
                        xcount++;
                    }
                    else{
                        xVar.add(QCPBarsData(lastX, xcount));
                        xcount = 1;
                        lastX = this->m_DIMMsample.xSeparation.at(i);

                    }
                    if(this->m_DIMMsample.ySeparation.at(i) == lastY){
                        ycount++;
                    }
                    else{
                        yVar.add(QCPBarsData(lastY, ycount));
                        ycount = 1;
                        lastY = this->m_DIMMsample.ySeparation.at(i);
                    }
                    if((this->m_DIMMsample.strehl1.at(i) < lastStrehl1 + 0.01 )&& (this->m_DIMMsample.strehl1.at(i) > lastStrehl1 - 0.01)){
                        strehlcount1++;
                    }
                    else{
                        strehl_distribution1.add(QCPBarsData(lastStrehl1, strehlcount1));
                        strehlcount1 = 1;
                        lastStrehl1 = this->m_DIMMsample.strehl1.at(i) ;
                    }
                    if((this->m_DIMMsample.strehl2.at(i) < lastStrehl2 + 0.0001 )&& (this->m_DIMMsample.strehl2.at(i) > lastStrehl2 - 0.0001)){
                        strehlcount2++;
                    }
                    else{
                        strehl_distribution2.add(QCPBarsData(lastStrehl2, strehlcount2));
                        strehlcount2 = 1;
                        lastStrehl2 = this->m_DIMMsample.strehl2.at(i) ;
                    }
                }
                this->plots.hVar.plottables.at(0).histogram->setWidth(1);
                this->plots.vVar.plottables.at(0).histogram->setWidth(1);
                //this->plots.strehl.plottables.at(0).histogram->setWidth(0.001);
                //this->plots.strehl.plottables.at(1).histogram->setWidth(0.001);

                this->m_seeingData.horizontalVarData->set(xVar);
                this->m_seeingData.verticalVarData->set(yVar);
                this->m_seeingData.strehlData_DIMM1->set(strehl_distribution1);
                this->m_seeingData.strehlData_DIMM2->set(strehl_distribution2);
            }
            emit newDIMMValues(); // updates the plots
            counter = 1; //reset counter and empty m_DIMMsample
            this->m_DIMMsample = {};        
        }
        if (counter == 1){//At the beginning of a sample, store the timestamp. If storeImages = true, create a folder where images can be stored.
            timestamp = this->m_imageContainer->startTime;

           //this->m_seeingValues.timestamps.push_back(this->m_imageContainer->startTime);
            if(this->m_configurationSettings.store_images){
                this->currentSubFolderName = this->timestampToFolderName(this->m_imageContainer->startTime);
                this->plots.saturation.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName +"/";
                this->plots.strehl.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                this->plots.vVar.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                this->plots.hVar.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentFolderPath + this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentFolderPath + this->currentSubFolderName, true );
                }
            }
            if(this->m_configurationSettings.debug){
                for(int i = 0; i< 256; i++){
                    (this->m_seeingData.saturation.data()->begin()+i)->value = 0.0;
                }
            }
        }

        if (!this->m_imageContainer->imgQueue.empty()){ //If there are images queued in the m_imagecontainer, process the oldest one.
            cv::Mat img = this->m_imageContainer->imgQueue.front();
            if (this->m_configurationSettings.store_images){
                if(!cv::imwrite( this->currentFolderPath.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".jpg", img)){
                    emit newMessage("Failed to save image", true);
                }
            }
            //Calculate the spotseparation of the image, then remove it from m_imageContainer.
            double x, y;
            cv::Point c1, c2;
            imageprocessing::getSpotSeparation(img, this->m_configurationSettings.window_radius, x,  y, c1, c2);
            this->m_DIMMsample.xSeparation.push_back(x);
            this->m_DIMMsample.ySeparation.push_back(y);
            /*

             * //if strehl less than cut off
            */

            if (this->m_configurationSettings.debug){
                double strehl1, strehl2;
                imageprocessing::getDIMM_strehlRatio(img, strehl1, strehl2, this->m_configurationSettings.pixFOV, this->m_configurationSettings.wavelength, this->m_configurationSettings.apertureDiameter, this->m_configurationSettings.window_radius);
                this->m_DIMMsample.strehl1.push_back(strehl1);
                this->m_DIMMsample.strehl2.push_back(strehl2);

                cv::Mat hist;
                imageprocessing::computeHistogram(img.clone(), hist);

                for(int i = 1; i < hist.rows; i++){
                    (this->m_seeingData.saturation.data()->begin()+i)->value += hist.at<float>(i);
                }
                this->m_DIMMsample.avgCentroid1 += c1;
                this->m_DIMMsample.avgCentroid2 += c2;
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


void SeeingGui::updateDIMMPlots(){
    this->plots.fried.replot();
    this->plots.seeing.replot();
    this->plots.fried.item->setText("Average Fried parameter: " + QString::number(this->getAverage(*this->m_seeingData.fried), 'f', 3) + " cm");
    this->plots.seeing.item->setText("Average seeing: " + QString::number(this->getAverage(*this->m_seeingData.seeing), 'f', 3) + " urad");
    if (this->m_configurationSettings.debug){
        this->plots.centroid.replot();
        this->plots.saturation.replot();
        this->plots.vVar.replot();
        this->plots.hVar.replot();
        this->plots.strehl.replot();

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
            if (this->m_configurationSettings.use_windowing){
                cv::Mat temp = this->m_GaussSample.gaussImg;
                imageprocessing::cropWindow(temp, this->m_GaussSample.gaussImg, this->m_configurationSettings.window_radius);
            }
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
                this->plots.saturation.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName +"/";
                this->plots.cm_fit.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                this->plots.cm_image.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                this->plots.cm_residue.save_settings.folder_path = this->currentFolderPath + this->currentSubFolderName+"/";
                if (!this->m_configurationSettings.baseDirectory.mkdir( this->currentFolderPath + this->currentSubFolderName ) ){
                    emit newMessage("Error while creating subdirectory: " + this->currentFolderPath +this->currentSubFolderName, true );
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
            else {
                croppedImg = img;
            }
            if (this->m_configurationSettings.debug){
                cv::Mat hist;
                imageprocessing::computeHistogram(croppedImg.clone(), hist);

                for(int i = 1; i < hist.rows; i++){
                    (this->m_seeingData.saturation.data()->begin()+i)->value += static_cast<double>(hist.at<float>(i));
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
                if(!cv::imwrite( this->currentFolderPath.toStdString() + this->currentSubFolderName.toStdString()  + "/img_" + std::to_string(counter) + ".png", img)){
                    emit newMessage("Failed to save image", true);
                }
                if(!cv::imwrite( this->currentFolderPath.toStdString() + this->currentSubFolderName.toStdString()  + "/gaussImg_" + std::to_string(counter) + ".png", this->m_GaussSample.gaussImg)){
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
    cv::Mat croppedImage;

    imageprocessing::cropWindow(image, croppedImage, this->m_configurationSettings.window_radius);
    datacontainers::gaussianFitParams fitParams = imageprocessing::getGaussianFitParams(croppedImage);

   /* if(true){
        for (int j = 0; j < image.cols; j++){
            std::cout << j << ": " << image.at<ushort>(cv::Point(fitParams.center_x, j)) << " ; " <<  croppedImage.at<ushort>(cv::Point(fitParams.center_x, j)) << " ; " <<  static_cast<int>(mask.at<uchar>(cv::Point(fitParams.center_x, j))) << " ; " <<  cropp2.at<ushort>(cv::Point(fitParams.center_x, j))<< std::endl;
        }
    }*/
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

    this->plots.cm_fit.plottable.colormap->setColorScale(this->plots.cm_image.plottable.colorscale);
    this->plots.cm_residue.plottable.colormap->setColorScale(this->plots.cm_image.plottable.colorscale);

    double x, y, zfit, xIm, yIm, zImage, xRes, yRes, zRes;
    for (int xIndex=0; xIndex<nx; ++xIndex)
    {

      for (int yIndex=0; yIndex<ny; ++yIndex)
      {
        this->plots.cm_image.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &xIm, &yIm);
        this->plots.cm_fit.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        this->plots.cm_residue.plottable.colormap->data()->cellToCoord(xIndex, yIndex, &xRes, &yRes);
        //std::cout << xIndex << " "<<  x <<" " <<  yIndex << " " << y << std::endl;
        zfit = exp(- (pow((xIndex-fitParams.center_x),2)/(2*fitParams.var_x)) - (pow((yIndex-fitParams.center_y),2)/(2*fitParams.var_y)));
        zImage = croppedImage.at<ushort>(cv::Point(xIndex, yIndex))/fitParams.intensitymax;
        //zRes = abs(zImage-zfit);
        zRes= abs((zfit-zImage)/zImage);
        if (zImage > 0.00001){
           // std::cout << "zfit: " << zfit << "\t" << "zimg: " << zImage << "\t"<< "res: " << zRes << std::endl;
        }
        this->plots.cm_image.plottable.colormap->data()->setCell(xIndex, yIndex, zImage);
        this->plots.cm_fit.plottable.colormap->data()->setCell(xIndex, yIndex, zfit);
        this->plots.cm_residue.plottable.colormap->data()->setCell(xIndex, yIndex, zRes);  
      }
    }
    this->plots.cm_image.replot();
    this->plots.cm_fit.replot();
    this->plots.cm_residue.replot();
    this->m_seeingData.crossSectionX.data()->set(QCPGraphDataContainer());
    this->m_seeingData.crossSectionY.data()->set(QCPGraphDataContainer());
    this->m_seeingData.crossSectionImageX.data()->set(QCPGraphDataContainer());
    this->m_seeingData.crossSectionImageY.data()->set(QCPGraphDataContainer());
    for(int i = 0; i<nx;i++){
        this->m_seeingData.crossSectionX->add(QCPGraphData(i, this->plots.cm_fit.plottable.colormap->data()->cell(i,static_cast<int>(fitParams.center_y))));
        this->m_seeingData.crossSectionImageX->add(QCPGraphData(i, this->plots.cm_image.plottable.colormap->data()->cell(i,static_cast<int>(fitParams.center_y))));
    }
    for(int i = 0; i<ny;i++){
        this->m_seeingData.crossSectionY->add(QCPGraphData(i, this->plots.cm_fit.plottable.colormap->data()->cell(static_cast<int>(fitParams.center_x),i)));
        this->m_seeingData.crossSectionImageY->add(QCPGraphData(i, this->plots.cm_image.plottable.colormap->data()->cell(static_cast<int>(fitParams.center_x),i)));
    }
    this->plots.gaussOverlap_x.replot();
    this->plots.gaussOverlap_y.replot();
}

void SeeingGui::updateGaussPlots(){
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
    this->parameterFile.setFileName(this->currentFolderPath + "SeeingParameters.txt");
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
        this->displayMessage("Error while creating parameter file: " + this->currentFolderPath + "SeeingParameters.txt", true);
    }
}

void SeeingGui::writeResultsToFile(){
    this->resultsFile.setFileName(this->currentFolderPath + "Results.txt");
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
    ///return fried parameter for DIMM measurement in units of cm
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


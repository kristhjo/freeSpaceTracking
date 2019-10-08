#include "trackinggui.h"
#include "ui_trackinggui.h"
#include "cameragui.h"
#include "seeinggui.h"
#include "hedylamarrgui.h"

TrackingGui::TrackingGui(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TrackingGui)
{
    ui->setupUi(this);

    this->isCameraConnected = false;
    this->isCameraRunning = false;
    this->isCameraTracking = false;
    this->isHexapodConnected = false;
    this->isHexapodStabilizing = false;
    this->isMeasuringSeeing = false;
    this->isHedyLamarrStabilizing = false;

    this->m_imageContainer = std::make_shared<datacontainers::ImageContainer>();
    this->centroidContainer = std::make_shared<datacontainers::CentroidStabilization>();
    this->centroidContainerHedy = std::make_shared<datacontainers::CentroidStabilization>();

    this->ui->HS_ExposureCam->setMinimum(15);
    this->ui->HS_ExposureCam->setMaximum(1000000);
    this->ui->SB_ExposureCam->setMinimum(15);
    this->ui->SB_ExposureCam->setMaximum(1000000);
    this->ui->SB_ExposureCam->setValue(100000);
    this->ui->HS_ExposureCam->setValue(100000);

    this->ui->SB_GainCam->setMinimum(1);
    this->ui->SB_GainCam->setMaximum(8);

    this->ui->SB_FrameRate->setMinimum(1);
    this->ui->SB_FrameRate->setMaximum(50);
    this->ui->SB_FrameRate->setValue(10);

    this->ui->HS_Width->setMinimum(5);
    this->ui->HS_Width->setMaximum(511);
    this->ui->HS_Width->setValue(60);
    this->ui->HS_Width->setSingleStep(1);

    this->ui->SB_Width->setMinimum(5);
    this->ui->SB_Width->setMaximum(511);
    this->ui->SB_Width->setValue(60);
    this->ui->SB_Width->setSingleStep(1);

    this->ui->HS_Height->setMinimum(5);
    this->ui->HS_Height->setMaximum(541);
    this->ui->HS_Height->setValue(100);
    this->ui->HS_Height->setSingleStep(1);

    this->ui->SB_Height->setMinimum(5);
    this->ui->SB_Height->setMaximum(541);
    this->ui->SB_Height->setValue(100);
    this->ui->SB_Height->setSingleStep(1);

    this->ui->HS_OffsetX->setMinimum(0);
    this->ui->HS_OffsetX->setMaximum(2044/4 - this->ui->HS_Width->value());
    this->ui->HS_OffsetX->setSingleStep(1);
    this->ui->HS_OffsetX->setValue(204);
    this->ui->SB_OffsetX->setMinimum(0);
    this->ui->SB_OffsetX->setMaximum(2044/4 - this->ui->HS_Width->value());
    this->ui->SB_OffsetX->setSingleStep(1);
    this->ui->SB_OffsetX->setValue(204);

    this->ui->HS_OffsetY->setMinimum(0);
    this->ui->HS_OffsetY->setMaximum(1084/2 - this->ui->HS_Height->value());
    this->ui->HS_OffsetY->setSingleStep(1);
    this->ui->HS_OffsetY->setValue(144);
    this->ui->SB_OffsetY->setMinimum(0);
    this->ui->SB_OffsetY->setMaximum(1084/2 - this->ui->HS_Height->value());
    this->ui->SB_OffsetY->setSingleStep(1);
    this->ui->SB_OffsetY->setValue(144);

    this->ui->SB_TrackingThresh->setMinimum(1);
    this->ui->SB_TrackingThresh->setMaximum(255);

    this->ui->SB_WindowRadius->setMinimum(1);
    this->ui->SB_WindowRadius->setMaximum(50);
    this->ui->SB_WindowRadius->setValue(10);

    QObject::connect(this->ui->PB_ConnectCam,&QPushButton::clicked,this,&TrackingGui::ConnectToCamera,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StopCam,&QPushButton::clicked,this,&TrackingGui::StopCamera,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StartCam,&QPushButton::clicked,this,&TrackingGui::StartCamera,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_Disconnect,&QPushButton::clicked,this,&TrackingGui::DisconnectFromCamera,Qt::UniqueConnection);

    QObject::connect(this->ui->HS_ExposureCam,&QSlider::sliderMoved,this,&TrackingGui::SetExposureTime);
    QObject::connect(this->ui->SB_ExposureCam,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetExposureTimeSB);
    QObject::connect(this->ui->SB_ExposureCam, SIGNAL(valueChanged(int)),this->ui->HS_ExposureCam, SLOT(setValue(int)));
    QObject::connect(this->ui->HS_ExposureCam, SIGNAL(valueChanged(int)),this->ui->SB_ExposureCam, SLOT(setValue(int)));

    QObject::connect(this->ui->SB_GainCam,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetGain,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_FrameRate,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetFrameRateSB,Qt::UniqueConnection);

    QObject::connect(this->ui->HS_Width,&QSlider::sliderMoved,this,&TrackingGui::SetWidth,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_Width,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetWidthSB,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_Width, SIGNAL(valueChanged(int)),this->ui->HS_Width, SLOT(setValue(int)));
    QObject::connect(this->ui->HS_Width, SIGNAL(valueChanged(int)),this->ui->SB_Width, SLOT(setValue(int)));

    QObject::connect(this->ui->HS_Height,&QSlider::sliderMoved,this,&TrackingGui::SetHeight,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_Height,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetWidthSB,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_Height, SIGNAL(valueChanged(int)),this->ui->HS_Height, SLOT(setValue(int)));
    QObject::connect(this->ui->HS_Height, SIGNAL(valueChanged(int)),this->ui->SB_Height, SLOT(setValue(int)));

    QObject::connect(this->ui->HS_OffsetX,&QSlider::sliderMoved,this,&TrackingGui::SetXOffset,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_OffsetX,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetXOffsetSB,Qt::UniqueConnection);
    QObject::connect(this->ui->HS_OffsetX, SIGNAL(valueChanged(int)),this->ui->SB_OffsetX, SLOT(setValue(int)));
    QObject::connect(this->ui->SB_OffsetX, SIGNAL(valueChanged(int)),this->ui->HS_OffsetX, SLOT(setValue(int)));

    QObject::connect(this->ui->HS_OffsetY,&QSlider::sliderMoved,this,&TrackingGui::SetYOffset,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_OffsetY,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::SetYOffsetSB,Qt::UniqueConnection);
    QObject::connect(this->ui->HS_OffsetY, SIGNAL(valueChanged(int)),this->ui->SB_OffsetY, SLOT(setValue(int)));
    QObject::connect(this->ui->SB_OffsetY, SIGNAL(valueChanged(int)),this->ui->HS_OffsetY, SLOT(setValue(int)));

    QObject::connect(this->ui->PB_StartTracking,&QPushButton::clicked,this,&TrackingGui::StartTracking,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StopTracking,&QPushButton::clicked,this,&TrackingGui::StopTracking,Qt::UniqueConnection);

    QObject::connect(this->ui->PB_UseThresholding,&QPushButton::clicked,this,&TrackingGui::useThresholding,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_TrackingThresh,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::setTrackingThreshold,Qt::UniqueConnection);

    QObject::connect(this->ui->PB_UseWindowing,&QPushButton::clicked,this,&TrackingGui::useWindowing,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_WindowRadius,QOverload<int>::of(&QSpinBox::valueChanged),this,&TrackingGui::setWindowRadius,Qt::UniqueConnection);

    QObject::connect(this->ui->PB_ConfigureSeeing, &QPushButton::clicked,this,&TrackingGui::ConfigureSeeing,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StartSeeingMeasurement, &QPushButton::clicked,this,&TrackingGui::StartSeeingMeasurement,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StopSeeingMeasurement, &QPushButton::clicked,this,&TrackingGui::StopSeeingMeasurement,Qt::UniqueConnection);

    QObject::connect(this->ui->PB_ConnectHexapod, &QPushButton::clicked, this, &TrackingGui::ConnectHexapod, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StartStabilization, &QPushButton::clicked, this, &TrackingGui::StartStabilization, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StopStabilization, &QPushButton::clicked, this, &TrackingGui::StopStabilization, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_ControlHexapod, &QPushButton::clicked, this, &TrackingGui::ManuallyControlHexapod, Qt::UniqueConnection);

    QObject::connect(this->ui->PB_ConnectHedyLamarr, &QPushButton::clicked, this, &TrackingGui::ConnectHedyLamarr, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StartHedyLamarrStabilization, &QPushButton::clicked, this, &TrackingGui::StartHedyLamarrStabilization, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_StopHedyLamarrStabilization, &QPushButton::clicked, this, &TrackingGui::StopHedyLamarrStabilization, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_DisconnectHedyLamarr, &QPushButton::clicked, this, &TrackingGui::DisconnectHedyLamarr, Qt::UniqueConnection);

}

TrackingGui::~TrackingGui()
{
    delete ui;
}

void TrackingGui::ConfigureSeeing(){
    if (!this->isCameraRunning.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Camera is not running \n");
        return;
    }
    if (this->isCameraTracking.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("You need to stop tracking to do a Seeing measurement  \n");
        return;
    }

    this->ui->TE_LogCam->append("Make sure to mount the meade twin-aperture cover and to use an appropriate exposure time\n");
    this->ui->TE_LogCam->append("Reminder: no rotation matrix is calculated in this software \n the baseline of the DIMM setup should be approximately parallel with the sensor x-axis for accurate calculations\n");

    //Initate the SeeingGui. Connect imageContainer and isMeasuringSeeing to CameraGui.
    this->pm_seeing =std::make_unique<SeeingGui>();
    this->pm_seeing->m_imageContainer = this->m_imageContainer;
    this->pm_seeing->isMeasuringSeeing = &this->isMeasuringSeeing;
    this->pm_seeing->m_seeingParams.exposureTime = this->pm_Camera->m_CamInfo.ExposureActual;
    this->pm_seeing->show();
}

void TrackingGui::StartSeeingMeasurement(){
    if(!this->pm_seeing){ //Force user to define measurement settings before activating a measurement.
        this->ui->TE_LogCam->append("You need to configure the seeing measurement first  \n");
        return;
    }
    this->pm_seeing->show();
    this->pm_seeing->startMeasurement();
    //Disables all but the stopMeasurement button in the main gui, so that camera parameters cannot be altered during a measurement.
    this->disableWidgets(true);
    this->ui->SeeingFrame->setEnabled(true);
    this->ui->PB_StopSeeingMeasurement->setEnabled(true);
}

void TrackingGui::StopSeeingMeasurement(){
    if(!this->isMeasuringSeeing.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("No seeing measurement is running  \n");
        this->disableWidgets(false);
        return;
    }
    this->pm_seeing->stopMeasurement();
    this->disableWidgets(false); //Reactivates all widgets.
}

void TrackingGui::disableWidgets(bool freeze){ //Loops through all "child" widgets of the trackinggui, and enables/disables them according to the boolean value of freeze.
    QList<QWidget*> widgetList = this->centralWidget()->findChildren<QWidget*>();
    foreach (auto obj, widgetList){
        obj->setEnabled(!freeze);
    }
}

void TrackingGui::ConnectHedyLamarr(){
    std::stringstream ss;
    if (this->isHedyLamarrConnected.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Hedy lamarr is already connected \n");
        return;
    }
    this->pm_hedylamarr = std::make_unique<HedyLamarrGui>(); //Initate the HedyLamarrGui.

    this->pm_hedylamarr->isHedyLamarrConnected = &this->isHedyLamarrConnected;
    this->pm_hedylamarr->isHedyLamarrStabilizing = &this->isHedyLamarrStabilizing;
    this->pm_hedylamarr->connectToHedyLamarr(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));
    this->pm_hedylamarr->show();
}

void TrackingGui::DisconnectHedyLamarr(){
    std::stringstream ss;
    if (!this->isHedyLamarrConnected.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Hedy lamarr is already disconnected \n");
        return;
    }

    if (this->isHedyLamarrStabilizing.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Can't disconnect during stabilization \n");
        return;
    }
    this->pm_hedylamarr->disconnectFromHedyLamarr(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));
}

void TrackingGui::StopHedyLamarrStabilization(){
    if (!this->isHedyLamarrStabilizing.load(std::memory_order_acquire)){ //if there exists an initiation of hexapodGui.
        this->ui->TE_LogCam->append("Hedy Lamarr is not stabilizing");
        return;
    }
    std::stringstream ss;
    this->pm_hedylamarr->stopStabilization(ss);
}

void TrackingGui::StartHedyLamarrStabilization(){
    if(!this->isHedyLamarrConnected){
        this->ui->TE_LogCam->append("Hedy Lamarr is not connected \n");
        return;
    }
    if (!this->isCameraTracking.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Camera is not tracking \n");
        return;
    }
    if(this->isHedyLamarrStabilizing){
        this->ui->TE_LogCam->append("Hedy Lamarr is already stabilizing \n");
        return;
    }
     this->pm_Camera->centroidContainerHedy = this->centroidContainerHedy; //Establishes shared connection between the centroidContainer of cameragui and hedylamarrgui.
    this->pm_hedylamarr->centroidContainer = this->centroidContainerHedy;

    std::stringstream ss;
    this->pm_hedylamarr->startStabilization(ss);
    this->pm_hedylamarr->show();
}

void TrackingGui::ConnectHexapod(){
    std::stringstream ss;
    if(this->isHexapodConnected){
        this->ui->TE_LogCam->append("Hexapod is already connected \n");
        return;
    }
    this->pm_hexapod = std::make_unique<HexapodGui>(); //Initate the HexapodGui.
    this->pm_hexapod->isHexapodConnected = &this->isHexapodConnected;
    this->pm_hexapod->isHexapodStabilizing = &this->isHexapodStabilizing;
    this->pm_hexapod->connectToHexapod(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));
    this->pm_hexapod->show();
}

void TrackingGui::StartStabilization(){
    if (!this->isHexapodConnected.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Hexapod is not connected \n");
        return;
    }
    if (!this->isCameraTracking.load(std::memory_order_acquire)){
        this->ui->TE_LogCam->append("Camera is not tracking \n");
        return;
    }
    if(this->isHexapodStabilizing){
        this->ui->TE_LogCam->append("Hexapod is already stabilizing \n");
        return;
    }
    this->pm_hexapod->setEnabled(false); //Deactivates hexapodgui so that gui position commands are unavailable during stabilization.
    this->ui->TE_LogCam->append("Deactivating keyboard control\n");
    this->pm_Camera->centroidContainer = this->centroidContainer; //Establishes shared connection between the centroidContainer of cameragui and hexpodgui.
    this->pm_hexapod->centroidContainer = this->centroidContainer;

    //this->pm_hexapod->updateRate = this->pm_Camera->m_CamInfo.ExposureActual;
    std::stringstream ss;
    this->pm_hexapod->startStabilization(ss);
    this->pm_hexapod->show();
}

void TrackingGui::StopStabilization(){
    if (!this->isHexapodStabilizing.load(std::memory_order_acquire)){ //if there exists an initiation of hexapodGui.
        this->ui->TE_LogCam->append("Hexapod is not stabilizing");
        return;
    }
    std::stringstream ss;
    this->pm_hexapod->stopStabilization(ss);
    this->pm_hexapod->setEnabled(true);
}


void TrackingGui::ManuallyControlHexapod(){
    if (!this->isHexapodConnected.load(std::memory_order_acquire)){ //if there exists an initiation of hexapodgui
        this->ui->TE_LogCam->append("Hexapod is not connected");
        return;
    }
    std::stringstream ss;
    this->pm_hexapod->activateManualControl(ss);
}

void TrackingGui::StartTracking(){
    if (!this->isCameraRunning){
        this->ui->TE_LogCam->append("Camera is not started \n");
        return;
    }
    this->isCameraTracking.store(true, std::memory_order_release); //Sets the flag in cameragui that makes it calculate centroids.
}

void TrackingGui::StopTracking(){
    if (!this->isCameraRunning){
        this->ui->TE_LogCam->append("Camera is not started \n");
        return;
    }
    this->isCameraTracking.store(false, std::memory_order_release);
  }

void TrackingGui::setTrackingThreshold(){
    if(!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_TrackingParameters.TrackingThresh = this->ui->SB_TrackingThresh->value();
}

void TrackingGui::useThresholding(){
    if(!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_TrackingParameters.useWindowing.store(false,std::memory_order_release);
}

void TrackingGui::useWindowing(){
    if(!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_TrackingParameters.useWindowing.store(true,std::memory_order_release);
}

void TrackingGui::setWindowRadius(){
    if(!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_TrackingParameters.WindowRadius = this->ui->SB_WindowRadius->value();//radius in pixels
}

void TrackingGui::SetExposureTime(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    bo_double exposureTime = this->ui->HS_ExposureCam->value();
    this->pm_Camera->m_CamInfo.ExposureActual = static_cast<double>(exposureTime);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetExposureTimeSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    bo_double exposureTime = this->ui->SB_ExposureCam->value();
    this->pm_Camera->m_CamInfo.ExposureActual = static_cast<double>(exposureTime);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetGain(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_CamInfo.GainActual = this->ui->SB_GainCam->value();
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetFrameRateSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->pm_Camera->m_CamInfo.FrameRate = this->ui->SB_FrameRate->value();
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}


void TrackingGui::SetWidth(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The frame width of the Baumer Cam can only change in increments of +- 4 pixels. Therefore a factor 4.
    this->pm_Camera->m_CamInfo.FrameWidth = this->ui->HS_Width->value()*4;
    this->ui->HS_OffsetX->setMaximum(2044/4 - this->pm_Camera->m_CamInfo.FrameWidth/4); //Ensures that the slider and the spin box hold the same value.
    this->ui->SB_OffsetX->setMaximum(2044/4 - this->pm_Camera->m_CamInfo.FrameWidth/4);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetWidthSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The frame width of the Baumer Cam can only change in increments of +- 4 pixels. Therefore a factor 4.
    this->pm_Camera->m_CamInfo.FrameWidth = this->ui->SB_Width->value()*4;
    this->ui->HS_OffsetX->setMaximum(2044/4 - this->pm_Camera->m_CamInfo.FrameWidth/4); //The maximal value of the horizontal offset depends on the frame width.
    this->ui->SB_OffsetX->setMaximum(2044/4 - this->pm_Camera->m_CamInfo.FrameWidth/4);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetXOffset(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The horizontal offset of the Baumer Cam can only change in increments of +- 4 pixels. Therefore a factor 4.
    this->pm_Camera->m_CamInfo.OffsetX = this->ui->HS_OffsetX->value()*4;
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetXOffsetSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The horizontal offset of the Baumer Cam can only change in increments of +- 4 pixels. Therefore a factor 4.
    this->ui->HS_OffsetX->setValue(this->ui->SB_OffsetX->value());
    this->pm_Camera->m_CamInfo.OffsetX = this->ui->HS_OffsetX->value()*4;
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetHeight(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The frame height of the Baumer Cam can only change in increments of +- 2 pixels. Therefore a factor 2.
    this->pm_Camera->m_CamInfo.FrameHeight = this->ui->HS_Height->value()*2;
    this->ui->HS_OffsetY->setMaximum(1084/2 - this->pm_Camera->m_CamInfo.FrameHeight/2); //The maximal value of the vertical offset depends on the frame height.
    this->ui->SB_OffsetY->setMaximum(1084/2 - this->pm_Camera->m_CamInfo.FrameHeight/2);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}
void TrackingGui::SetHeightSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The frame height of the Baumer Cam can only change in increments of +- 2 pixels. Therefore a factor 2.
    this->pm_Camera->m_CamInfo.FrameHeight = this->ui->SB_Height->value()*2;
    this->ui->HS_OffsetY->setMaximum(1084/2 - this->pm_Camera->m_CamInfo.FrameHeight/2); //The maximal value of the vertical offset depends on the frame height.
    this->ui->SB_OffsetY->setMaximum(1084/2 - this->pm_Camera->m_CamInfo.FrameHeight/2);
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::SetYOffsetSB(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    this->ui->HS_OffsetY->setValue(this->ui->SB_OffsetY->value());
    // The vertical offset of the Baumer Cam can only change in increments of +- 2 pixels. Therefore a factor 2.
    this->pm_Camera->m_CamInfo.OffsetY = this->ui->HS_OffsetY->value()*2;
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}
void TrackingGui::SetYOffset(){
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    // The vertical offset of the Baumer Cam can only change in increments of +- 2 pixels. Therefore a factor 2.
    this->pm_Camera->m_CamInfo.OffsetY = this->ui->HS_OffsetY->value()*2;
    this->pm_Camera->newCamParameter.store(true,std::memory_order_release);
}

void TrackingGui::ConnectToCamera(){

    std::stringstream ss;
    if (this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is already connected \n");
        return;
    }

    if(this->pm_Camera!=nullptr){
        this->pm_Camera.reset();
    }
    this->pm_Camera=std::make_unique<CameraGui>(); //Initiates the CameraGui.
    this->pm_Camera->m_CamInfo.BaumerID = this->ui->CB_Baumer->currentText().toStdString();
    this->pm_Camera->m_imageContainer = this->m_imageContainer;
    this->pm_Camera->isCameraTracking = &this->isCameraTracking;
    this->pm_Camera->isCameraRunning = &this->isCameraRunning;
    this->pm_Camera->isCameraConnected = &this->isCameraConnected;
    this->pm_Camera->isMeasuringSeeing = &this->isMeasuringSeeing;
    this->pm_Camera->isHexapodStabilizing = &this->isHexapodStabilizing;
    this->pm_Camera->isHedyLamarrStabilizing = &this->isHedyLamarrStabilizing;
    this->pm_Camera->m_TrackingParameters.useWindowing.store(true,std::memory_order_release);
    this->pm_Camera->Connect(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));

}

void TrackingGui::StartCamera(){
    std::stringstream ss;
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is disconnected \n");
        return;
    }
    else if (this->isCameraRunning){
        this->ui->TE_LogCam->append("Camera is already running \n");
        return;
    }
    //update cam parameters with standard settings
    this->pm_Camera->m_CamInfo.OffsetY = this->ui->HS_OffsetY->value()*2;
    this->pm_Camera->m_CamInfo.OffsetX = this->ui->HS_OffsetX->value()*4;
    this->pm_Camera->m_CamInfo.FrameHeight = this->ui->HS_Height->value()*2;
    this->pm_Camera->m_CamInfo.FrameWidth = this->ui->HS_Width->value()*4;
    this->pm_Camera->m_CamInfo.ExposureActual= this->ui->HS_ExposureCam->value();
    //
    this->pm_Camera->Start(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));
    this->isCameraRunning = true;
}

void TrackingGui::StopCamera(){
    std::stringstream ss;
    if (!this->isCameraRunning){
        this->ui->TE_LogCam->append("Camera has not been started \n");
        return;
    }
    this->pm_Camera->Stop(ss);
    this->ui->TE_LogCam->append(QString::fromStdString(ss.str()));
    this->isCameraRunning = false;
}

void TrackingGui::DisconnectFromCamera(){
    std::stringstream ss;
    if (!this->isCameraConnected){
        this->ui->TE_LogCam->append("Camera is not connected \n");
        return;
    }
    else if (this->isCameraRunning) {
        this->ui->TE_LogCam->append("Stopping camera \n");
        this->ui->TE_LogCam->update();
        this->StopCamera();
    }
    this->pm_Camera->Disconnect(ss);
}

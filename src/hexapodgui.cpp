#include <QKeyEvent>
#include "hexapodgui.h"
#include "ui_hexapodgui.h"
#include "datacontainers.h"
#include "PI_GCS2_DLL.h"
#include <iomanip>
#include <time.h>
#include <algorithm>

HexapodGui::HexapodGui(QWidget *parent) :
    QMainWindow (parent),
    ui(new Ui::HexapodGui)
{
    ui->setupUi(this);

    this->ui->TE_LogHexapod->setReadOnly(true);
    QDir dir;
    dir.cdUp();
    this->projectFolderPath = dir.path();
    QObject::connect(this->ui->HS_IncrementSize, &QSlider::sliderMoved, this, &HexapodGui::setStepSize, Qt::UniqueConnection ) ;
    QObject::connect(this->ui->SB_StepSize,QOverload<int>::of(&QSpinBox::valueChanged),this,&HexapodGui::setStepSizeSB, Qt::UniqueConnection);
    QObject::connect(this->ui->HS_IncrementSize, SIGNAL(valueChanged(int)),this->ui->SB_StepSize, SLOT(setValue(int)));
    QObject::connect(this->ui->SB_StepSize, SIGNAL(valueChanged(int)),this->ui->HS_IncrementSize, SLOT(setValue(int)));

    QObject::connect(this->ui->PB_setUserDefinedCoSys, &QPushButton::clicked,this,&HexapodGui::setUserDefinedCoSys, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_setZeroCoSys, &QPushButton::clicked,this,&HexapodGui::setZeroCoSys, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_ZeroOnActiveCoSys, &QPushButton::clicked,this,&HexapodGui::moveToZero, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_MoveToPosition, &QPushButton::clicked, this,&HexapodGui::moveToDisplayPosition, Qt::UniqueConnection );

    QObject::connect(this, &HexapodGui::positionChanged, this, &HexapodGui::updateDisplayPositions);
    QObject::connect(this, &HexapodGui::coSysChanged, this, &HexapodGui::updateCoSysDisplay);
    QObject::connect(this, SIGNAL(updateStabilizationPlot()), this, SLOT(update_plotsWindow()));

    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(newPositionMessage(QString)), this, SLOT(displayPosition(QString)));

    QObject::connect(this->ui->SB_deadTime,QOverload<int>::of(&QSpinBox::valueChanged),this,&HexapodGui::setDeadTime,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_maxMotion,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&HexapodGui::setMaxMotion,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_proportionalGain,QOverload<double>::of(&QDoubleSpinBox::valueChanged),this,&HexapodGui::setProportionalGain,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_integrationTime,QOverload<int>::of(&QSpinBox::valueChanged),this,&HexapodGui::setIntegrationTime,Qt::UniqueConnection);
}

void HexapodGui::setDeadTime(){
    this->m_configurationSettings.deadTime = this->ui->SB_deadTime->value();
}

void HexapodGui::setMaxMotion(){
    this->m_configurationSettings.maxMotion = this->ui->SB_maxMotion->value();
}

void HexapodGui::setProportionalGain(){
    this->m_configurationSettings.proportionalGain = this->ui->SB_proportionalGain->value();
}

void HexapodGui::setIntegrationTime(){
    this->m_configurationSettings.integrationTime = this->ui->SB_integrationTime->value();
}

HexapodGui::~HexapodGui(){
    if(!this->isHexapodStabilizing->load(std::memory_order_acquire)){ //don't want to disconnect hexapod during stabilization in case of accidental delete of gui.
        this->isHexapodConnected->store(false, std::memory_order_release);
        PI_CloseConnection(this->ID);
    }
    delete ui;
}

void HexapodGui::loadConfiguration(QString configurationFilePath){
    QSettings settings(configurationFilePath, QSettings::IniFormat);
    settings.beginGroup("HexapodGui");
    QString ipadr = settings.value("Hexapod_ipadr").toString();
    //this->m_configurationSettings.ipadr = ipadr.toStdString().c_str();//.c_str();//toLocal8Bit().constData();
    this->m_configurationSettings.ipadr = ipadr;//.c_str();//toLocal8Bit().constData();

    std::cout << "in load " << this->m_configurationSettings.ipadr.toStdString() << " "<< this->m_configurationSettings.port << std::endl;
    this->m_configurationSettings.integrationTime = settings.value("integrationTime",1.0).toDouble();
    this->m_configurationSettings.deadTime = settings.value("deadTime",1.0).toDouble();
    this->m_configurationSettings.proportionalGain = settings.value("proportionalGain",0.5).toDouble();
    this->m_configurationSettings.maxMotion = settings.value("maxMotion",0.1).toDouble();
    this->m_configurationSettings.horizontalAxis = settings.value("horizontalAxis","W").toString();
    this->m_configurationSettings.verticalAxis = settings.value("verticalAxis","V").toString();
    this->m_configurationSettings.horizontalSign = settings.value("horizontalSign").toInt();
    this->m_configurationSettings.verticalSign = settings.value("verticalSign").toInt();
    this->m_configurationSettings.storage_location = settings.value("storage_location", this->projectFolderPath + "/results/stabilization/").toString();

    this->m_configurationSettings.horizontalAxisIndex = this->hexapodParams.coAxisMap[this->m_configurationSettings.horizontalAxis];
    this->m_configurationSettings.verticalAxisIndex = this->hexapodParams.coAxisMap[this->m_configurationSettings.verticalAxis];
    settings.endGroup();
    int errorValue = 0;
    settings.beginGroup("TrackingOptics");
    this->m_configurationSettings.magnification_telescope = settings.value("magnification_telescope", errorValue).toDouble();
    this->m_configurationSettings.pixel_size = settings.value("pixel_size", errorValue).toDouble();
    this->m_configurationSettings.focalLength_eyepiece = settings.value("focalLength_eyepiece", errorValue).toDouble();
    if(this->m_configurationSettings.magnification_telescope == errorValue || this->m_configurationSettings.pixel_size == errorValue || this->m_configurationSettings.focalLength_eyepiece == errorValue){
        this->newMessage("Warning: it seems like the settings of the SeeingOptics was not set correctly in the .ini file. This will cause errors later.", true);
    }
    this->m_configurationSettings.update_pix_FoV();
    this->configureSettings();

}
void HexapodGui::configureSettings(){

    this->ui->SB_deadTime->setValue(this->m_configurationSettings.deadTime);
    this->ui->SB_integrationTime->setValue(this->m_configurationSettings.integrationTime);
    this->ui->SB_proportionalGain->setValue(this->m_configurationSettings.proportionalGain);
    this->ui->SB_maxMotion->setValue(this->m_configurationSettings.maxMotion);
}


void HexapodGui::setup_plots_window(){
    this->plotsWindow = std::make_unique<plotDisplay>();
    this->m_stabilizationPlots.stabilization.save_settings = savingSettings("", this->currentFolderPath, true, "pdf");
    this->plotsWindow->add_plot(this->m_stabilizationPlots.stabilization);
    this->m_stabilizationPlots.stabilization.add_plottable(scatterPlotConfig(this->m_stabilizationData.hexapod_vertical,"vertical motion", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 15),QPen(QColor(255,140,0))));
    this->m_stabilizationPlots.stabilization.add_plottable(scatterPlotConfig(this->m_stabilizationData.hexapod_horizontal,"horizontal motion", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssSquare, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 15),QPen(QColor(10, 140, 70))));
    this->m_stabilizationPlots.stabilization.add_plottable(scatterPlotConfig(this->m_stabilizationData.centroid_vertical,"vertical centroid dev.", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(QColor(255,140,0)), 15),QPen(QColor(255,140,0)),QCPAxis::atRight));
    this->m_stabilizationPlots.stabilization.add_plottable(scatterPlotConfig(this->m_stabilizationData.centroid_horizontal,"horizontal centroid dev.", QCPGraph::lsLine, QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black, 1.5), QBrush(QColor(10, 140, 70)), 15),QPen(QColor(10, 140, 70)),QCPAxis::atRight));
    this->plotsWindow->show();
}



void HexapodGui::connectToHexapod(std::stringstream &ss){
    this->ID = PI_ConnectTCPIP (this->m_configurationSettings.ipadr.toStdString().c_str(), this->m_configurationSettings.port );
    if ( this->ID < 0 ){
        ss << "connection to hexapod failed \n";
        this->isHexapodConnected->store(false, std::memory_order_release);
        return;
    }
    this->isHexapodConnected->store(true, std::memory_order_release);

    this->installEventFilter(this->keyPressEater);  //Installs an event filter for keyboard input, which is deactivated during manual control.
    if(!this->referenceIfNeeded()){
        emit newMessage("Couldn't perform reference move", true);
    }

    PI_KEN(this->ID, "ZERO");

    PI_qPOS(this->ID, "X Y Z U V W", this->currentPos);
    PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos);
    emit positionChanged();
    emit coSysChanged();

}

void HexapodGui::activateManualControl(std::stringstream &ss){
    this->removeEventFilter(this->keyPressEater);
    ss << "Activated keyboard control of hexapod";
}

void HexapodGui::startStabilization(std::stringstream &ss){
    this->installEventFilter(this->keyPressEater); //Prevents manual control of the hexapod through keyboard input.
    ss << "Deactivated keyboard control of hexapod";
    this->isHexapodStabilizing->store(true, std::memory_order_release);
    ss << "Hexapod stabilization activated";
    //create directory q
    QDir basedir;
    this->folderName = QDateTime::fromTime_t(static_cast<uint>(time(nullptr))).toString(Qt::TextDate) + "/";
    this->currentFolderPath = this->m_configurationSettings.storage_location + this->folderName;
    if(!basedir.mkpath(this->m_configurationSettings.storage_location + this->folderName) ){

        emit newMessage("Couldn't create folder " + this->m_configurationSettings.storage_location + this->folderName, true);
    }
    this->stabilizationDataStream.open(this->m_configurationSettings.storage_location.toStdString() + this->folderName.toStdString() + "/stabilizationData.dat",std::fstream::out|std::fstream::app);
    if(!stabilizationDataStream.is_open())
    {
        std::cout <<this->folderName.toStdString() + "/stabilizationData.dat" <<"\t File could not be created " << std::endl;
        this->isHexapodStabilizing->store(false, std::memory_order_release);
        return;
    }
    this->setup_plots_window();
    this->disableCommands(true);
    //set home centroid
    std::this_thread::sleep_for(std::chrono::seconds(3));
    this->centroidContainer->updateMeanCentroid();
    this->centroidContainer->emptyHistory();
    this->centroidContainer->homeX = this->centroidContainer->meanCentroidX;
    this->centroidContainer->homeY = this->centroidContainer->meanCentroidY;
    this->stabilize.store(true, std::memory_order_release);
    this->stabilizationLoop = std::make_unique<std::thread>(&HexapodGui::Stabilize,this); //Initiates a separate thread with the real-time stabilization
}

void HexapodGui::stopStabilization(std::stringstream &ss){
    this->stabilize.store(false, std::memory_order_release);
    while(this->stabilizationInProcess.load(std::memory_order_acquire)){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        emit newMessage("Waiting for stabilizationLoop to finish");
    }
    this->isHexapodStabilizing->store(false, std::memory_order_release);
    this->stabilizationDataStream.close();
    if(this->stabilizationLoop->joinable()){
        ss << "Closing stabilization thread.";
        this->stabilizationLoop->join();
    }
    else{
        ss << "Serious thread error occured.";
        return;
    }
    ss << "Hexapod stabilization turned off.";

    this->disableCommands(false);
    return;
}

void HexapodGui::Stabilize(){
    this->stabilizationInProcess.store(true, std::memory_order_release);
    while(this->stabilize.load(std::memory_order_acquire)){

        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(this->m_configurationSettings.integrationTime))); //WAITS FOR NEW CENTROID MEASUREMENTS.
        this->centroidContainer->updateMeanCentroid();
        this->centroidContainer->emptyHistory();

        double dHorizontal = this->centroidContainer->meanCentroidX - this->centroidContainer->homeX; //horizontal pixel deviation from home position
        double dVertical = this->centroidContainer->meanCentroidY - this->centroidContainer->homeY; //vertical pixel deviation from home position

        double newPos[6] = { -this->currentPos[0], -this->currentPos[1], -this->currentPos[2], 0, 0, 0}; //subtract any movement along other axes than V,W, that may have occured from previous rotations.

        double horizontalMotion = dHorizontal*this->m_configurationSettings.pixToHex*this->m_configurationSettings.proportionalGain*this->m_configurationSettings.horizontalSign;
        double verticalMotion = dVertical*this->m_configurationSettings.pixToHex*this->m_configurationSettings.proportionalGain*this->m_configurationSettings.verticalSign;

        time_t current_time = time(nullptr);
        if(abs(horizontalMotion)< this->m_configurationSettings.maxMotion){
            newPos[this->m_configurationSettings.horizontalAxisIndex] += horizontalMotion;
            this->m_stabilizationData.hexapod_horizontal->add(QCPGraphData(current_time, this->currentPos[this->m_configurationSettings.horizontalAxisIndex]*this->m_configurationSettings.degToRad*1e6)); //convert to micro radians
            this->m_stabilizationData.centroid_horizontal->add(QCPGraphData(current_time, dHorizontal));

        }
        if(abs(verticalMotion)< this->m_configurationSettings.maxMotion){
            newPos[this->m_configurationSettings.verticalAxisIndex] += verticalMotion;
            this->m_stabilizationData.hexapod_vertical->add(QCPGraphData(current_time, this->currentPos[this->m_configurationSettings.verticalAxisIndex]*this->m_configurationSettings.degToRad*1e6)); //convert to micro radians
            this->m_stabilizationData.centroid_vertical->add(QCPGraphData(current_time, dVertical));

        }
        else{
            std::cout << "too far deviation " << verticalMotion << std::endl;
        }
        this->MoveToPosition( newPos, true ); //True marks that hexapod moves relative to current position in the Work Coordinate System.
        emit updateStabilizationPlot();
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(this->m_configurationSettings.deadTime))); //WAITS FOR NEW CENTROID MEASUREMENTS.

    }
    this->stabilizationInProcess.store(false, std::memory_order_release);

}

void HexapodGui::update_plotsWindow(){
    this->m_stabilizationPlots.stabilization.replot();

    this->stabilizationDataStream << (this->m_stabilizationData.hexapod_vertical->constEnd()-1)->key << "\t" << (this->m_stabilizationData.hexapod_vertical->constEnd()-1)->value << "\t" << (this->m_stabilizationData.hexapod_horizontal->constEnd()-1)->value << "\t" << (this->m_stabilizationData.centroid_vertical->constEnd()-1)->value << "\t" << (this->m_stabilizationData.centroid_horizontal->constEnd()-1)->value << "\n";
    this->stabilizationDataStream.flush();
    //only plot points from the last 5 min.
    this->m_stabilizationData.hexapod_vertical->removeBefore(time(nullptr) - 300);
    this->m_stabilizationData.hexapod_horizontal->removeBefore(time(nullptr) - 300);
    this->m_stabilizationData.centroid_vertical->removeBefore(time(nullptr) - 300);
    this->m_stabilizationData.centroid_horizontal->removeBefore(time(nullptr) - 300);

}
void HexapodGui::disableCommands(bool disable){ //Loops through all "child" widgets of the trackinggui, and enables/disables them according to the boolean value of freeze.
    QList<QWidget*> widgetList = this->centralWidget()->findChildren<QWidget*>();
    foreach (auto obj, widgetList){
        obj->setEnabled(!disable);
    }
    this->ui->SB_deadTime->setEnabled(true);
    this->ui->SB_proportionalGain->setEnabled(true);
    this->ui->SB_maxMotion->setEnabled(true);
    this->ui->SB_integrationTime->setEnabled(true);
}


bool HexapodGui::referenceIfNeeded(){
    BOOL bReferenced;
    BOOL bFlag;
    char axis[] = "X";
    if ( !PI_qFRF ( this->ID, axis, &bReferenced ) ){
        return false;
    }
    if ( !bReferenced ) {// if needed, reference the axis using the refence switch
        printf ( "Referencing axis %s...\n", axis );
        if ( !PI_FRF ( this->ID, axis ) ){
            return false;
        }

        // Wait until the reference move is done.
        bFlag = false;
        while ( TRUE != bFlag )
        {
            if ( !PI_IsControllerReady ( this->ID, &bFlag ) )
                return false;
        }
    }
    return true;
}

void HexapodGui::MoveToPosition (double *dVal, bool moveRelative)
{
    BOOL bMovePossible = false;
    if (!moveRelative){ //if dVal is specified in absolute coordinates, check if dVal is a possible position.
        PI_qVMO ( this->ID, "X Y Z U V W", dVal, &bMovePossible );
    }
    else { //if dVal is specified as a relative move to the current position, calculate absolute position first.
        double absPos[6];
        for (int i = 0; i < 6 ; i++){
            absPos[i] = this->currentPos[i] + dVal[i];
        }
        PI_qVMO ( this->ID, "X Y Z U V W", absPos, &bMovePossible ); //I’m uncertain if qVMO is aware of active coordinate system.
    }

    if (bMovePossible){
        char endPos[200];
        std::sprintf (endPos,"Moving to position (%s): %f,%f,%f,%f,%f,%f ", "X Y Z U V W", dVal[0], dVal[1], dVal[2], dVal[3], dVal[4], dVal[5] );
        emit newPositionMessage(endPos);

        if (!moveRelative){
            if ( !PI_MOV ( this->ID, "X Y Z U V W", dVal) ){            //move to absolute position in the active coordinate system
                PI_STP(this->ID);
                emit newMessage("Error during request to move. Stopped all motion.", true);
                return;
            }
        }
        else {
            if ( !PI_MRW ( this->ID, "X Y Z U V W", dVal) ){        //move relative to current position in the active coordinate system
                PI_STP(this->ID);
                emit newMessage("Error during request to move. Stopped all motion.", true);
                return;
            }
        }
        BOOL bIsMoving[1] = { TRUE };
        double dPos[6] = { 0, 0, 0, 0, 0, 0};
        this->isStabMoving.store(true, std::memory_order_release);
        while ( bIsMoving[0] == TRUE ) //while at least one axis is moving
        {
            if ( !PI_qPOS ( this->ID, "X Y Z U V W", dPos ) ){  //update position. Ends motion in case of query error
                PI_STP(this->ID);
                emit newMessage("Error during position query. Stopped all motion.", true);
                return;
            }
            if ( !PI_IsMoving ( this->ID, "", bIsMoving ) ){ //update bIsMoving, ends motion in case of query error
                PI_STP(this->ID);
                emit newMessage("Error during isMoving query. Stopped all motion.", true);
                return;
            }
        }
        emit newPositionMessage("############ Position reached ############\n" );
        this->isStabMoving.store(false, std::memory_order_release);
    }
    else {
        char endPos[200];
        std::sprintf (endPos,"Couldn't move to position (%s): %f,%f,%f,%f,%f,%f ", "X Y Z U V W", dVal[0], dVal[1], dVal[2], dVal[3], dVal[4], dVal[5] );
        emit newMessage(endPos, true);
        return;
    }
    PI_qPOS(this->ID, "X Y Z U V W", this->currentPos);
    emit positionChanged(); //update displays in gui.
    return;
}

void HexapodGui::setStepSize(){
    this->stepSize = this->ui->HS_IncrementSize->value();
}

void HexapodGui::setStepSizeSB(){
    this->stepSize = this->ui->SB_StepSize->value();
}

void HexapodGui::setZeroCoSys(){
    this->pauseUntilHexapodStops();

    if (!PI_KEN(this->ID, "ZERO")){
        this->displayMessage("failed to activate ZERO");
        return;
    };
    for (int i = 0; i < 6; i++){
        this->currentCoSys[i] = 0.0;
    }
    this->activeCoordinateSystem = "ZERO";
    PI_qPOS(this->ID, "X Y Z U V W", this->currentPos);
    PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos);
    emit coSysChanged();
    emit positionChanged();
    this->displayMessage("ZERO is now activated");
    double dMaxTravelPositive[6];
    PI_qTMX ( ID, "X Y Z U V W", dMaxTravelPositive );
    printf("Max range (%s): %f,%f,%f,%f,%f,%f ", "X Y Z U V W", dMaxTravelPositive[0], dMaxTravelPositive[1], dMaxTravelPositive[2], dMaxTravelPositive[3], dMaxTravelPositive[4], dMaxTravelPositive[5]);
}

void HexapodGui::moveToZero(){
    double zeroPos[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    this->displayMessage("Moving to origo in " + QString::fromStdString(this->activeCoordinateSystem) + " Coordinate system.");
    MoveToPosition(zeroPos, false); //move in absolute coordinates relative active coordinate system.
}
void HexapodGui::pauseUntilHexapodStops(){
    BOOL bIsMoving[1] = {TRUE};
    while ( bIsMoving[0] ){ //while at least one axis is moving
        if ( !PI_IsMoving ( this->ID, "", bIsMoving ) ){
            emit newMessage("error in PI_IsMoving query", true);
            return;
        }
    }
}

void HexapodGui::setUserDefinedCoSys(){
    this->pauseUntilHexapodStops();
    double newCoSysPosition[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    PI_KEN(this->ID, "ZERO"); //changing temporarily to ZERO so USERDEFINED can be changed. It is only possible to change inactive coordinate systems.
    PI_qPOS(this->ID, "X Y Z U V W", newCoSysPosition); //get position of the USERDEFINED coordinate system in the ZERO coordinate system.
    PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos); //get current absolute position in ZERO co.sys.

    if(!PI_KSF(this->ID, "USERDEFINED")){
        emit newMessage("Could not define USERDEFINED coordinate system", true);
        return;
    }
    if (!PI_KEN(this->ID, "USERDEFINED")){
        emit newMessage("Could not activate USERDEFINED coordinate system", true);
        return;
    }
    emit newMessage("USERDEFINED coordinate system activated");
    this->activeCoordinateSystem = "USERDEFINED";
    std::copy(newCoSysPosition, newCoSysPosition + 6, this->currentCoSys);
    PI_qPOS(this->ID, "X Y Z U V W", this->currentPos);
    emit coSysChanged();
    emit positionChanged();
    double dMaxTravelPositive[6];
    double dir[6] = {0, 0, 0, 0, 1, 1};
    PI_qTRA ( ID, "X Y Z U V W", dir, dMaxTravelPositive );
    printf("Max range (%s): %f,%f,%f,%f,%f,%f ", "X Y Z U V W", dMaxTravelPositive[0], dMaxTravelPositive[1], dMaxTravelPositive[2], dMaxTravelPositive[3], dMaxTravelPositive[4], dMaxTravelPositive[5]);
}

void HexapodGui::keyPressEvent(QKeyEvent * event)
{   //currently configured to move relative
    if (event->isAutoRepeat()){
        return;
    }
    double newPos[6] = { 0, 0, 0, 0, 0, 0};
    bool move = false;

    if (event->key() == Qt::Key_W){
        //move up Y
        newPos[1] += this->stepSize*this->hexapodParams.dY;
        move = true;
    }
    if (event->key() == Qt::Key_S){
        //move down Y
        newPos[1] -=  this->stepSize*this->hexapodParams.dY;
        move = true;
    }
    if (event->key() == Qt::Key_A){
        //increase X
         newPos[0] +=  this->stepSize*this->hexapodParams.dX;
         move = true;
    }
    if (event->key() == Qt::Key_D){
        //decrease X
        newPos[0] -=  this->stepSize*this->hexapodParams.dX;
        move = true;
    }
    if (event->key() == Qt::Key_Q){
        //increase Z
        newPos[2] +=  this->stepSize*this->hexapodParams.dZ;
        move = true;
    }
    if (event->key() == Qt::Key_E){
        //decrease Z
        newPos[2] -=  this->stepSize*this->hexapodParams.dZ;
        move = true;
    }
    if (event->key() == Qt::Key_U){
        //increase U
        newPos[3] +=  this->stepSize*this->hexapodParams.dU;
        move = true;
    }
    if (event->key() == Qt::Key_I){
        //decrease U
        newPos[3] -=  this->stepSize*this->hexapodParams.dU;
        move = true;
    }
    if (event->key() == Qt::Key_V){
        //increase U
        newPos[4] +=  this->stepSize*this->hexapodParams.dV;
        move = true;
    }
    if (event->key() == Qt::Key_B){
        //decrease U
        newPos[4] -=  this->stepSize*this->hexapodParams.dV;
        move = true;
    }
    if (event->key() == Qt::Key_K){
        //increase W
        newPos[5] +=  this->stepSize*this->hexapodParams.dW;
        move = true;
    }
    if (event->key() == Qt::Key_L){
        //decrease W
        newPos[5] -=  this->stepSize*this->hexapodParams.dW;
        move = true;
    }
    if(move){
        BOOL bMovePossible = false;
        PI_qVMO ( this->ID, "X Y Z U V W", newPos, &bMovePossible );
        if (bMovePossible){
            bool moveRelative = true;
            this->MoveToPosition( newPos, moveRelative);
        }
    }
}
void HexapodGui::moveToDisplayPosition(){
    double newPos[6];
    newPos[0] = this->ui->LE_xMove->text().toDouble();
    newPos[1] = this->ui->LE_yMove->text().toDouble();
    newPos[2] = this->ui->LE_zMove->text().toDouble();
    newPos[3] = this->ui->LE_uMove->text().toDouble();
    newPos[4] = this->ui->LE_vMove->text().toDouble();
    newPos[5] = this->ui->LE_wMove->text().toDouble();

    MoveToPosition( newPos, false );
}

void HexapodGui::updateCoSysDisplay(){
    this->ui->LE_xActiveCo->setText(QString::number(this->currentCoSys[0], this->positionFormat, this->positionPrecision));
    this->ui->LE_yActiveCo->setText(QString::number(this->currentCoSys[1], this->positionFormat, this->positionPrecision));
    this->ui->LE_zActiveCo->setText(QString::number(this->currentCoSys[2], this->positionFormat, this->positionPrecision));
    this->ui->LE_uActiveCo->setText(QString::number(this->currentCoSys[3], this->positionFormat, this->positionPrecision));
    this->ui->LE_vActiveCo->setText(QString::number(this->currentCoSys[4], this->positionFormat, this->positionPrecision));
    this->ui->LE_wActiveCo->setText(QString::number(this->currentCoSys[5], this->positionFormat, this->positionPrecision));
}

void HexapodGui::updateDisplayPositions(){
    this->ui->LE_xToolCo->setText(QString::number(this->currentPos[0], this->positionFormat, this->positionPrecision));
    this->ui->LE_yToolCo->setText(QString::number(this->currentPos[1], this->positionFormat, this->positionPrecision));
    this->ui->LE_zToolCo->setText(QString::number(this->currentPos[2], this->positionFormat, this->positionPrecision));
    this->ui->LE_uToolCo->setText(QString::number(this->currentPos[3], this->positionFormat, this->positionPrecision));
    this->ui->LE_vToolCo->setText(QString::number(this->currentPos[4], this->positionFormat, this->positionPrecision));
    this->ui->LE_wToolCo->setText(QString::number(this->currentPos[5], this->positionFormat, this->positionPrecision));

    if(this->activeCoordinateSystem == "ZERO"){
        PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos);
    }
    else{ //switch temporarily to the ZERO coordinate system to get absolute coordinates
        PI_KEN(this->ID, "ZERO");
        PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos);
        PI_KEN(this->ID, "USERDEFINED");
    }
    this->ui->LE_xAbsCo->setText(QString::number(this->currentAbsPos[0], this->positionFormat, this->positionPrecision));
    this->ui->LE_yAbsCo->setText(QString::number(this->currentAbsPos[1], this->positionFormat, this->positionPrecision));
    this->ui->LE_zAbsCo->setText(QString::number(this->currentAbsPos[2], this->positionFormat, this->positionPrecision));
    this->ui->LE_uAbsCo->setText(QString::number(this->currentAbsPos[3], this->positionFormat, this->positionPrecision));
    this->ui->LE_vAbsCo->setText(QString::number(this->currentAbsPos[4], this->positionFormat, this->positionPrecision));
    this->ui->LE_wAbsCo->setText(QString::number(this->currentAbsPos[5], this->positionFormat, this->positionPrecision));
}

void HexapodGui::displayMessage(QString message, bool error){
    this->ui->TE_LogHexapod->append(message);
    this->ui->TE_LogHexapod->update();
    if (error){
        qDebug() << message;
    }
}
void HexapodGui::displayPosition(QString message){
    this->ui->TE_LivePosition->append(message);
    this->ui->TE_LivePosition->update();
}


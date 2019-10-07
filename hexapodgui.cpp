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

    this->ui->HS_IncrementSize->setMinimum(1);
    this->ui->HS_IncrementSize->setMaximum(10000);
    this->ui->HS_IncrementSize->setValue(1);
    this->ui->TE_LogHexapod->setReadOnly(true);

    QObject::connect(this->ui->HS_IncrementSize, &QSlider::sliderMoved, this, &HexapodGui::setStepSize, Qt::UniqueConnection ) ;
    QObject::connect(this->ui->PB_setUserDefinedCoSys, &QPushButton::clicked,this,&HexapodGui::setUserDefinedCoSys, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_setZeroCoSys, &QPushButton::clicked,this,&HexapodGui::setZeroCoSys, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_ZeroOnActiveCoSys, &QPushButton::clicked,this,&HexapodGui::moveToZero, Qt::UniqueConnection);
    QObject::connect(this->ui->PB_MoveToPosition, &QPushButton::clicked, this,&HexapodGui::moveToDisplayPosition, Qt::UniqueConnection );
    QObject::connect(this, &HexapodGui::positionChanged, this, &HexapodGui::updateDisplayPositions);
    QObject::connect(this, &HexapodGui::coSysChanged, this, &HexapodGui::updateCoSysDisplay);
    QObject::connect(this, SIGNAL(updateStabilizationPlot()), this, SLOT(updatePlot()));
    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(newPositionMessage(QString)), this, SLOT(displayPosition(QString)));
}

HexapodGui::~HexapodGui(){
    if(!this->isHexapodStabilizing->load(std::memory_order_acquire)){ //don't want to disconnect hexapod during stabilization in case of accidental delete of gui.
        this->isHexapodConnected->store(false, std::memory_order_release);
        PI_CloseConnection(this->ID);
    }
    delete ui;
}

void HexapodGui::connectToHexapod(std::stringstream &ss){
    this->ID = PI_ConnectTCPIP (this->hexapodParams.ipadr, this->hexapodParams.port);
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
//  PI_qVLS (this->ID, &this->velocity);

    PI_qPOS(this->ID, "X Y Z U V W", this->currentPos);
    PI_qPOS(this->ID, "X Y Z U V W", this->currentAbsPos);
    emit positionChanged();
    emit coSysChanged();
    //char buf[3000];
    //PI_qPUN(this->ID, "X Y Z U V W", buf, 3000);
    //std::cout << buf << std::endl;
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
    this->initPlot();
    //create directory q
    QDir basedir;
    time_t startTime = time(nullptr);
    QDateTime t(QDateTime::fromTime_t(startTime));
    this->folderName = t.toString();
    if(!basedir.mkdir(folderName)){
        emit newMessage("Couldn't create folder for plots", true);
    }
    this->stabilizationDataStream.open(this->folderName.toStdString() + "/stabilizationData.dat",std::fstream::out|std::fstream::app);
    if(!stabilizationDataStream.is_open())
    {
        std::cout <<this->folderName.toStdString() + "/stabilizationData.dat" <<"\t File could not be created " << std::endl;
        this->isHexapodStabilizing->store(false, std::memory_order_release);
        return;
    }
    //set home centroid
    std::this_thread::sleep_for(std::chrono::seconds(3));
    this->centroidContainer->updateMeanCentroid();
    this->centroidContainer->emptyHistroy();
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
    return;
}

void HexapodGui::Stabilize(){
    this->stabilizationInProcess.store(true, std::memory_order_release);
    double maxMovePerUpdate = 0.2;
    while(this->stabilize.load(std::memory_order_acquire)){

        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(this->updateRate))); //WAITS FOR NEW CENTROID MEASUREMENTS.
        this->centroidContainer->updateMeanCentroid();
        this->centroidContainer->emptyHistroy();


        double dHorizontal = this->centroidContainer->meanCentroidX - this->centroidContainer->homeX; //horizontal pixel deviation from home position
        double dVertical = this->centroidContainer->meanCentroidY - this->centroidContainer->homeY; //vertical pixel deviation from home position
        this->xCentroids.push_back(dHorizontal);
        this->yCentroids.push_back(dVertical);
        if (abs(dHorizontal) < 2){
            dHorizontal = 0;
        }
        if (abs(dVertical) < 2){
            dVertical = 0;
        }

        double newPos[6] = { -this->currentPos[0], -this->currentPos[1], -this->currentPos[2], -this->currentPos[3], 0, 0}; //subtract any movement along other axes than V,W, that may have occured from previous rotations.

        newPos[5] -= dHorizontal*this->hexapodParams.pixToHex;

        newPos[4] += dVertical*this->hexapodParams.pixToHex;

        if ( (abs(newPos[4])  > maxMovePerUpdate) || (abs(newPos[5]) > maxMovePerUpdate) ){
            continue;
        }
        this->MoveToPosition( newPos, true ); //True marks that hexapod moves relative to current position in the Work Coordinate System.
        //this->isStabMoving.load(std::memory_order_acquire);


        this->horizontalTilts.push_back(this->currentPos[4]*1e6*this->degToRad); //convert to micro radians
        this->verticalTilts.push_back(this->currentPos[5]*1e6*this->degToRad); //convert to micro radians
        this->timestamps.push_back(time(nullptr));
        emit updateStabilizationPlot();
    }
    this->stabilizationInProcess.store(false, std::memory_order_release);
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

void HexapodGui::initPlot(){
    //set the time format of the x-axis.
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    this->ui->StabilizationPlot->clearGraphs(); // clear old graphs
    this->ui->StabilizationPlot->plotLayout()->clear();
    this->plotData_hTilt.clear();
    this->plotData_vTilt.clear();
    this->plotData_hPix.clear();
    this->plotData_vPix.clear();
    dateTicker->setDateTimeFormat("hh:mm:ss");
    QFont titleFont =  QFont("sans", 12, QFont::Bold);
    QFont legendFont =  QFont("sans", 8, QFont::Bold);
    QFont axisFont = QFont("sans", 12, QFont::Bold);
    QPen pixelPen;
    QPen tiltPen;
    pixelPen.setStyle(Qt::DotLine);
    pixelPen.setWidthF(2);
    tiltPen.setStyle(Qt::DotLine);
    tiltPen.setWidthF(2);

    this->ui->StabilizationPlot->addGraph(this->ui->StabilizationPlot->xAxis, this->ui->StabilizationPlot->yAxis);
    tiltPen.setColor(Qt::blue);
    this->ui->StabilizationPlot->graph(0)->setPen(tiltPen);
    this->ui->StabilizationPlot->graph(0)->setName(QString("Horizontal tilt"));

    this->ui->StabilizationPlot->addGraph(this->ui->StabilizationPlot->xAxis, this->ui->StabilizationPlot->yAxis);
    tiltPen.setColor(Qt::red);
    this->ui->StabilizationPlot->graph(0)->setPen(tiltPen);
    this->ui->StabilizationPlot->graph(1)->setName(QString("Vertical tilt"));

    this->ui->StabilizationPlot->addGraph(this->ui->StabilizationPlot->xAxis2, this->ui->StabilizationPlot->yAxis2);
    pixelPen.setColor(Qt::red);
    this->ui->StabilizationPlot->graph(2)->setPen(pixelPen);
    this->ui->StabilizationPlot->graph(2)->setBrush(QBrush(Qt::red));
    this->ui->StabilizationPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    this->ui->StabilizationPlot->graph(2)->setName(QString("Vertical pixel offset"));

    this->ui->StabilizationPlot->addGraph(this->ui->StabilizationPlot->xAxis2, this->ui->StabilizationPlot->yAxis2);
    pixelPen.setColor(Qt::blue);
    this->ui->StabilizationPlot->graph(2)->setPen(pixelPen);
    this->ui->StabilizationPlot->graph(3)->setBrush(QBrush(Qt::blue));
    this->ui->StabilizationPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    this->ui->StabilizationPlot->graph(3)->setName(QString("Horizontal pixel offset"));

    this->ui->StabilizationPlot->xAxis->setLabel("");
    this->ui->StabilizationPlot->xAxis->setTicker(dateTicker);
    this->ui->StabilizationPlot->xAxis->setLabelFont(axisFont);
    this->ui->StabilizationPlot->yAxis->setLabel("[urad]");
    this->ui->StabilizationPlot->yAxis->setLabelFont(axisFont);
    this->ui->StabilizationPlot->yAxis2->setVisible(true);
    this->ui->StabilizationPlot->yAxis2->setLabel("Pixels");
    this->ui->StabilizationPlot->yAxis2->setLabelFont(axisFont);
    this->ui->StabilizationPlot->plotLayout()->insertRow(0);
    this->ui->StabilizationPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->StabilizationPlot, "Stabilization History", titleFont));

    this->ui->StabilizationPlot->legend->setVisible(true);
    this->ui->StabilizationPlot->legend->setFont(legendFont);
    this->ui->StabilizationPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));

    this->ui->StabilizationPlot->replot();
}

void HexapodGui::updatePlot(){
    QCPGraphData dataPoint(this->timestamps.last(), this->horizontalTilts.last());
    this->plotData_hTilt.push_back(dataPoint);
    if(abs(horizontalTilts.last())>this->maxRange_tilt){
        this->maxRange_tilt = abs(horizontalTilts.last());
    }
    if(abs(xCentroids.last())>this->maxRange_px){
        this->maxRange_px = abs(xCentroids.last());
    }
    dataPoint.value  = this->verticalTilts.last();
    this->plotData_vTilt.push_back(dataPoint);
    dataPoint.value  = this->xCentroids.last();
    this->plotData_hPix.push_back(dataPoint);
    dataPoint.value  = this->yCentroids.last();
    this->plotData_vPix.push_back(dataPoint);

    if (this->plotData_hTilt.size() > this->xAxisLimit){
        this->plotData_hTilt.erase(this->plotData_hTilt.begin());
        this->plotData_vTilt.erase(this->plotData_vTilt.begin());
        this->plotData_hPix.erase(this->plotData_hPix.begin());
        this->plotData_vPix.erase(this->plotData_vPix.begin());
    }


    this->ui->StabilizationPlot->yAxis->setRange(-this->maxRange_tilt*1.2, this->maxRange_tilt*1.2);
    this->ui->StabilizationPlot->yAxis2->setRange(-this->maxRange_px*1.2, this->maxRange_px*1.2);
    this->ui->StabilizationPlot->rescaleAxes();
    this->ui->StabilizationPlot->graph(0)->data()->set(plotData_hTilt);
    this->ui->StabilizationPlot->graph(1)->data()->set(plotData_vTilt);
    this->ui->StabilizationPlot->graph(2)->data()->set(plotData_vPix);
    this->ui->StabilizationPlot->graph(3)->data()->set(plotData_hPix);

    this->ui->StabilizationPlot->replot();
    this->ui->StabilizationPlot->savePdf(this->folderName + "/StabilizationPlot.pdf" );

    this->stabilizationDataStream << this->timestamps.last() << "\t" << this->xCentroids.last() << "\t" << this->yCentroids.last() << "\t" << this->horizontalTilts.last() << "\t" << this->verticalTilts.last() << "\n";
    this->stabilizationDataStream.flush();
}

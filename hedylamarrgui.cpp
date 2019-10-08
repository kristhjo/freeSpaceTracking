#include "hedylamarrgui.h"
#include "ui_hedylamarrgui.h"
#include "datacontainers.h"
#include <iomanip>
#include <time.h>
#include <algorithm>
#include <QHostAddress>

HedyLamarrGui::HedyLamarrGui(QWidget *parent) :
    QMainWindow (parent),
    ui(new Ui::HedyLamarrGui)
{
    ui->setupUi(this);
    this->ui->TE_LogHedyLamarr->setReadOnly(true);
    this->NSoffset = 0.0;
    this->EWoffset = 0.0;
    this->ui->SB_stepSize->setMinimum(1);
    this->ui->SB_stepSize->setMaximum(3600);
    this->ui->SB_stepSize->setValue(5);

    QObject::connect(this, SIGNAL(newMessage(QString, bool)), this, SLOT(displayMessage(QString, bool)));
    QObject::connect(this, SIGNAL(updateStabilizationPlot()), this, SLOT(updatePlot()));
    QObject::connect(this, &HedyLamarrGui::newCentroid, this, &HedyLamarrGui::updateDisplay);
    QObject::connect(this->ui->PB_North, &QPushButton::clicked,this,&HedyLamarrGui::moveNorth,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_South, &QPushButton::clicked,this,&HedyLamarrGui::moveSouth,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_West, &QPushButton::clicked,this,&HedyLamarrGui::moveWest,Qt::UniqueConnection);
    QObject::connect(this->ui->PB_East, &QPushButton::clicked,this,&HedyLamarrGui::moveEast,Qt::UniqueConnection);
    QObject::connect(this->ui->SB_stepSize, QOverload<int>::of(&QSpinBox::valueChanged),this,&HedyLamarrGui::SetStepSize);
    QObject::connect(this, SIGNAL(newCommand(QString)), this, SLOT(sendCommand(QString)), Qt::AutoConnection);
}

HedyLamarrGui::~HedyLamarrGui(){
    if(this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){ //don't want to disconnect hedy lamarr during stabilization in case of accidental delete of gui.
        this->displayMessage("Can't close window during stabilization");
    }
    else {
        this->hedylamarr_socket.disconnectFromHost();
        this->isHedyLamarrConnected->store(false, std::memory_order_release);
        delete ui;
    }
}

void HedyLamarrGui::connectToHedyLamarr(std::stringstream &ss){
    this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
    if(this->hedylamarr_socket.waitForConnected(3000)){ //timeout at 3 sec
        ss << "connection to hedy lamarr established \n";
        this->isHedyLamarrConnected->store(true, std::memory_order_release);
    }
    else {
        ss << "connection to hedy lamarr failed \n";
        this->isHedyLamarrConnected->store(false, std::memory_order_release);
    }
    this->setCurrentOffset();
}
void HedyLamarrGui::disconnectFromHedyLamarr(std::stringstream &ss){
    this->hedylamarr_socket.disconnectFromHost();
    if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState ||
            this->hedylamarr_socket.waitForDisconnected(3000)){
        ss << "disconnected from hedy lamarr \n";
        this->isHedyLamarrConnected->store(false, std::memory_order_release);
    }
    else {
        ss << "failed to disconnect from hedy lamarr\n";
        this->isHedyLamarrConnected->store(true, std::memory_order_release);
    }
}

void HedyLamarrGui::startStabilization(std::stringstream &ss){
    this->isHedyLamarrStabilizing->store(true, std::memory_order_release);
    ss << "Hedy Lamarr stabilization activated";
    this->initPlot();
    //create directory
    QDir basedir;
    QDateTime startTime(QDateTime::fromTime_t(time(nullptr)));
    this->folderName = "HedyLamarr " + startTime.toString();
    if(!basedir.mkdir(folderName)){
        emit newMessage("Couldn't create folder for plots", true);
    }
    this->stabilizationDataStream.open(this->folderName.toStdString() + "/stabilizationData.dat",std::fstream::out|std::fstream::app);
    if(!stabilizationDataStream.is_open())
    {
        std::cout <<this->folderName.toStdString() + "/stabilizationData.dat" <<"\t File could not be created " << std::endl;
        this->isHedyLamarrStabilizing->store(false, std::memory_order_release);
        return;
    }
    //set home centroid
    std::this_thread::sleep_for(std::chrono::seconds(3));
    this->centroidContainer->updateMeanCentroid();
    this->centroidContainer->emptyHistroy();
    this->centroidContainer->homeX = this->centroidContainer->meanCentroidX;
    this->centroidContainer->homeY = this->centroidContainer->meanCentroidY;
    this->ui->LE_xHomeCentroid->setText(QString::number(this->centroidContainer->homeX, this->positionFormat, this->positionPrecision));
    this->ui->LE_yHomeCentroid->setText(QString::number(this->centroidContainer->homeY, this->positionFormat, this->positionPrecision));

    this->stabilize.store(true, std::memory_order_release);
    this->stabilizationLoop = std::make_unique<std::thread>(&HedyLamarrGui::Stabilize,this); //Initiates a separate thread with the real-time stabilization
}


void HedyLamarrGui::Stabilize(){
    this->stabilizationInProcess.store(true, std::memory_order_release);
    double maxPixMovPerUpdate = 5;
    while(this->stabilize.load(std::memory_order_acquire)){

        std::this_thread::sleep_for(std::chrono::seconds(this->updateRate)); //WAITS FOR NEW CENTROID MEASUREMENTS.
        this->centroidContainer->updateMeanCentroid();
        this->centroidContainer->emptyHistroy();
        emit newCentroid();
        double dHorizontal = this->centroidContainer->meanCentroidX - this->centroidContainer->homeX; //horizontal pixel deviation from home position
        double dVertical = this->centroidContainer->meanCentroidY - this->centroidContainer->homeY; //vertical pixel deviation from home position

        double newNS = 0.0;
        double newEW = 0.0;

        if (abs(dHorizontal) < maxPixMovPerUpdate){
            newNS = dVertical*this->HedyLamarrParams.pixToHedyLamarr;
        }
        else if (dHorizontal > maxPixMovPerUpdate){//If deviation is larger than the distance the hexapod can move in between centroid updates, move maxMovPerUpdate instead.
            newNS = maxPixMovPerUpdate*this->HedyLamarrParams.pixToHedyLamarr;
        }
        else if (dHorizontal < -maxPixMovPerUpdate){
            newNS = -maxPixMovPerUpdate*this->HedyLamarrParams.pixToHedyLamarr;
        }
        if (abs(dVertical) < maxPixMovPerUpdate){
            newEW = dVertical*this->HedyLamarrParams.pixToHedyLamarr;
        }
        else if (dVertical > maxPixMovPerUpdate){//If deviation is larger than the distance the hexapod can move in between centroid updates, move maxMovPerUpdate instead.
            newEW = maxPixMovPerUpdate*this->HedyLamarrParams.pixToHedyLamarr;
        }
        else if (dVertical < -maxPixMovPerUpdate){
            newEW = -maxPixMovPerUpdate*this->HedyLamarrParams.pixToHedyLamarr;
        }
        this->NSoffset += newNS;
        this->EWoffset += newEW;
        std::cout << newEW << " " << newNS << std::endl;
        QString NScommand = "$SOR,0000000056,94,2,41=51:164,164=48:" + QString::number(NSoffset, 'f', 6) + ",$EOM,$EOR";
        emit newCommand(NScommand);
        QString EWcommand = "$SOR,0000000056,94,2,41=51:163,163=48:"+  QString::number(EWoffset, 'f', 6) + ",$EOM,$EOR";
        emit newCommand(EWcommand);
        QCPGraphData HTilt(time(nullptr), dHorizontal*this->HedyLamarrParams.pixFieldOfView);//this->HedyLamarrParams.pixToHedyLamarr/this->HedyLamarrParams.radToArcSec);
        QCPGraphData VTilt(time(nullptr), dVertical*this->HedyLamarrParams.pixFieldOfView);//*this->HedyLamarrParams.pixToHedyLamarr/this->HedyLamarrParams.radToArcSec);
        QCPGraphData XDev(time(nullptr), dHorizontal);
        QCPGraphData YDev(time(nullptr), dVertical);

        this->plotData_hTilt.push_back(HTilt);
        this->plotData_vTilt.push_back(VTilt);
        this->plotData_hPix.push_back(XDev);
        this->plotData_vPix.push_back(YDev);
        emit updateStabilizationPlot();
    }
    this->stabilizationInProcess.store(false, std::memory_order_release);
}


void HedyLamarrGui::stopStabilization(std::stringstream &ss){
    this->stabilize.store(false, std::memory_order_release);
    while(this->stabilizationInProcess.load(std::memory_order_acquire)){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        emit newMessage("Waiting for stabilizationLoop to finish");
    }
    this->isHedyLamarrStabilizing->store(false, std::memory_order_release);
    this->stabilizationDataStream.close();
    if(this->stabilizationLoop->joinable()){
        ss << "Closing stabilization thread.";
        this->stabilizationLoop->join();
    }
    else{
        ss << "Serious thread error occured.";
        return;
    }
    ss << "Hedy Lamarr stabilization turned off.";
}

void HedyLamarrGui::moveNorth(){
    if(!this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){
        if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
            this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
        }
        this->NSoffset += this->ui->SB_stepSize->value();
        QString command = "$SOR,0000000056,94,2,41=51:164,164=48:" + QString::number(NSoffset, 'f', 6) + ",$EOM,$EOR";
        this->hedylamarr_socket.write(command.toUtf8());
        this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
    }
    else{
        this->displayMessage("Can't use manual commands while stabilizing");
    }
}

void HedyLamarrGui::moveSouth(){
    if(!this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){
        if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
            this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
        }
        this->NSoffset -= this->ui->SB_stepSize->value();
        QString command = "$SOR,0000000056,94,2,41=51:164,164=48:"+ QString::number(NSoffset, 'f', 6) + ",$EOM,$EOR";
        this->hedylamarr_socket.write(command.toUtf8());
         this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
    }
    else{
        this->displayMessage("Can't use manual commands while stabilizing");
    }
}


void HedyLamarrGui::moveWest(){
    if(!this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){
        if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
            this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
        }
        this->EWoffset += this->ui->SB_stepSize->value();
        QString command = "$SOR,0000000056,94,2,41=51:163,163=48:"+  QString::number(EWoffset, 'f', 6)  + ",$EOM,$EOR";
        this->hedylamarr_socket.write(command.toUtf8());
        this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
    }
    else{
        this->displayMessage("Can't use manual commands while stabilizing");
    }
}

void HedyLamarrGui::moveEast(){
    if(!this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){
        if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
            this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
        }
        this->EWoffset -= this->ui->SB_stepSize->value();
        QString command = "$SOR,0000000056,94,2,41=51:163,163=48:"+  QString::number(EWoffset, 'f', 6) + ",$EOM,$EOR";
        this->hedylamarr_socket.write(command.toUtf8());
        this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
    }
    else{
        this->displayMessage("Can't use manual commands while stabilizing");
    }
}

void HedyLamarrGui::SetStepSize(){
    this->stepSize = this->ui->SB_stepSize->value();
}

void HedyLamarrGui::sendCommand(QString command){
    if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
        this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
    }
    this->hedylamarr_socket.write(command.toUtf8());
    this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
}

void HedyLamarrGui::setCurrentOffset(){
    if (this->hedylamarr_socket.state() == QAbstractSocket::UnconnectedState){
        this->hedylamarr_socket.connectToHost(QHostAddress("127.0.0.1"),2400);
    }
    QString NScommand = "$SOR,0000000040,93,1,41=51:164,$EOM,$EOR";
    this->hedylamarr_socket.write(NScommand.toUtf8());
    std::this_thread::sleep_for(std::chrono::seconds(this->updateRate));

    QString NSresponse = QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll());
    std::cout << NSresponse.toStdString() <<std::endl;
    this->displayResponse(NSresponse);
    QString EWcommand = "$SOR,0000000040,93,1,41=51:163,$EOM,$EOR";
    this->hedylamarr_socket.write(EWcommand.toUtf8());
    QString EWresponse = QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll());
    std::cout << EWresponse.mid(38,8).toDouble() <<std::endl;
    this->displayResponse(EWresponse);
}

void HedyLamarrGui::displayMessage(QString message, bool error){
     this->ui->TE_LogHedyLamarr->append(message);
     this->ui->TE_LogHedyLamarr->update();
     if (error){
         qDebug() << message;
     }
}

void HedyLamarrGui::updateDisplay(){
    this->ui->LE_xLastCentroid->setText(QString::number(this->centroidContainer->meanCentroidX, this->positionFormat, this->positionPrecision));
    this->ui->LE_yLastCentroid->setText(QString::number(this->centroidContainer->meanCentroidX, this->positionFormat, this->positionPrecision));
}
void HedyLamarrGui::displayResponse(QString response){
    this->ui->TE_HedyResponse->append(response);
    this->ui->TE_HedyResponse->update();
}
void HedyLamarrGui::initPlot(){
    //set the time format of the x-axis.
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss");
    //clear plot if necessary
    //this->ui->StabilizationPlot->plotLayout()->clear();
    this->ui->StabilizationPlot->clearGraphs();
    this->plotData_hTilt.clear();
    this->plotData_vTilt.clear();
    this->plotData_hPix.clear();
    this->plotData_vPix.clear();
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
    //this->ui->StabilizationPlot->graph(2)->setBrush(QBrush(Qt::red));
    this->ui->StabilizationPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 5));
    this->ui->StabilizationPlot->graph(2)->setName(QString("Vertical pixel offset"));

    this->ui->StabilizationPlot->addGraph(this->ui->StabilizationPlot->xAxis2, this->ui->StabilizationPlot->yAxis2);
    pixelPen.setColor(Qt::blue);
    this->ui->StabilizationPlot->graph(2)->setPen(pixelPen);
    //this->ui->StabilizationPlot->graph(3)->setBrush(QBrush(Qt::blue));
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

void HedyLamarrGui::updatePlot(){

    if(this->plotData_hTilt.size() > this->maxXRangePlot){
        this->plotData_hTilt.pop_front();
        this->plotData_vTilt.pop_front();
        this->plotData_hPix.pop_front();
        this->plotData_vPix.pop_front();
    }

    this->ui->StabilizationPlot->yAxis->setRange(-this->maxYRangePlot, this->maxYRangePlot);
    this->ui->StabilizationPlot->rescaleAxes();
    this->ui->StabilizationPlot->graph(0)->data()->set(plotData_hTilt);
    this->ui->StabilizationPlot->graph(1)->data()->set(plotData_vTilt);
    this->ui->StabilizationPlot->graph(2)->data()->set(plotData_vPix);
    this->ui->StabilizationPlot->graph(3)->data()->set(plotData_hPix);

    this->ui->StabilizationPlot->replot();
    this->ui->StabilizationPlot->savePdf(this->folderName + "/HedyLamarrStabilizationPlot.pdf" );

    this->stabilizationDataStream << this->plotData_hTilt.last().key << "\t" << this->plotData_hPix.last().value << "\t" << this->plotData_vPix.last().value << "\t" << this->plotData_hTilt.last().value << "\t" << this->plotData_vTilt.last().value << "\n";
    this->stabilizationDataStream.flush();
}



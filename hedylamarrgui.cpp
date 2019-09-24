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

        std::this_thread::sleep_for(std::chrono::seconds(static_cast<int>(3))); //WAITS FOR NEW CENTROID MEASUREMENTS.
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
        newNS = 1;
        newEW = 1;
        this->NSoffset += newNS;
        this->EWoffset += newEW;
        std::cout << newEW << " " << newNS << std::endl;
        QString NScommand = "$SOR,0000000056,94,2,41=51:164,164=48:" + QString::number(NSoffset, 'f', 6) + ",$EOM,$EOR";
        //this->hedylamarr_socket.write(NScommand.toUtf8());
        //this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
        emit newCommand(NScommand);
        QString EWcommand = "$SOR,0000000056,94,2,41=51:163,163=48:"+  QString::number(EWoffset, 'f', 6) + ",$EOM,$EOR";
        //this->hedylamarr_socket.write(EWcommand.toUtf8());
        //this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
        emit newCommand(EWcommand);
        QCPGraphData HTilt(time(nullptr), dHorizontal*this->HedyLamarrParams.pixToHedyLamarr*1000);
        QCPGraphData VTilt(time(nullptr), dVertical*this->HedyLamarrParams.pixToHedyLamarr*1000);
        QCPGraphData XDev(time(nullptr), dHorizontal);
        QCPGraphData YDev(time(nullptr), dVertical);

        this->plotDataHTilt.push_back(HTilt);
        this->plotDataVTilt.push_back(VTilt);
        this->plotDataXDeviation.push_back(XDev);
        this->plotDataYDeviation.push_back(YDev);
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
        this->NSoffset -= this->ui->SB_stepSize->value();
        QString command = "$SOR,0000000056,94,2,41=51:164,164=48:"+ QString::number(NSoffset, 'f', 6) + ",$EOM,$EOR";
        this->hedylamarr_socket.write(command.toUtf8());
        //char response[200];
        //qint64 sizeOfResponse = this->hedylamarr_socket.readLine(response, 200);
        //std::cout << QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()).toStdString() << std::endl;
        this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
    }
    else{
        this->displayMessage("Can't use manual commands while stabilizing");
    }
}


void HedyLamarrGui::moveWest(){
    if(!this->isHedyLamarrStabilizing->load(std::memory_order_acquire)){
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
    this->hedylamarr_socket.write(command.toUtf8());
    this->displayResponse(QTextCodec::codecForMib(106)->toUnicode(this->hedylamarr_socket.readAll()));
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
    this->ui->StabilizationPlot->clearGraphs();
    //
    this->ui->StabilizationPlot->addGraph();
    this->ui->StabilizationPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    this->ui->StabilizationPlot->graph(0)->setName(QString("W"));
    this->ui->StabilizationPlot->xAxis->setLabel("");
    this->ui->StabilizationPlot->xAxis->setTicker(dateTicker);
    this->ui->StabilizationPlot->yAxis->setLabel("[urad]");
    this->ui->StabilizationPlot->plotLayout()->insertRow(0);
    this->ui->StabilizationPlot->plotLayout()->addElement(0, 0, new QCPTextElement(this->ui->StabilizationPlot, "Stabilization History"));
    this->ui->StabilizationPlot->addGraph();
    this->ui->StabilizationPlot->graph(1)->setName(QString("V"));
    this->ui->StabilizationPlot->graph(1)->setPen(QPen(Qt::red));
    this->ui->StabilizationPlot->legend->setVisible(true);
    this->ui->StabilizationPlot->replot();

    this->ui->StabilizationPlot->addGraph();
    this->ui->StabilizationPlot->graph(2)->setName(QString("Y"));
    this->ui->StabilizationPlot->graph(2)->setPen(QPen(Qt::red));
    this->ui->StabilizationPlot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->ui->StabilizationPlot->legend->setVisible(true);

    this->ui->StabilizationPlot->addGraph();
    this->ui->StabilizationPlot->graph(3)->setName(QString("X"));
    this->ui->StabilizationPlot->graph(3)->setPen(QPen(QColor(40, 110, 255)));
    this->ui->StabilizationPlot->graph(3)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 5));
    this->ui->StabilizationPlot->legend->setVisible(true);

    this->ui->StabilizationPlot->replot();
}

void HedyLamarrGui::updatePlot(){

    if(this->plotDataHTilt.size() > this->maxXRangePlot){
        this->plotDataHTilt.pop_front();
        this->plotDataVTilt.pop_front();
        this->plotDataXDeviation.pop_front();
        this->plotDataYDeviation.pop_front();
    }

    this->ui->StabilizationPlot->yAxis->setRange(-this->maxYRangePlot, this->maxYRangePlot);
    this->ui->StabilizationPlot->rescaleAxes();
    this->ui->StabilizationPlot->graph(0)->data()->set(plotDataHTilt);
    this->ui->StabilizationPlot->graph(1)->data()->set(plotDataVTilt);
    this->ui->StabilizationPlot->graph(2)->data()->set(plotDataYDeviation);
    this->ui->StabilizationPlot->graph(3)->data()->set(plotDataXDeviation);

    this->ui->StabilizationPlot->replot();
    this->ui->StabilizationPlot->savePdf(this->folderName + "/HedyLamarrStabilizationPlot.pdf" );

    this->stabilizationDataStream << this->plotDataHTilt.last().key << "\t" << this->plotDataXDeviation.last().value << "\t" << this->plotDataYDeviation.last().value << "\t" << this->plotDataHTilt.last().value << "\t" << this->plotDataVTilt.last().value << "\n";
    this->stabilizationDataStream.flush();
}



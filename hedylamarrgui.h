#ifndef HEDYLAMARRGUI_H
#define HEDYLAMARRGUI_H

#include <iostream>
#include <QMainWindow>
#include <QWidget>
#include "PI_GCS2_DLL.h"
#include "datacontainers.h"
#include <opencv2/opencv.hpp>
#include <QVector>
#include <sys/socket.h>
#include <QTcpSocket>
#include <QHostAddress>
namespace Ui {
class HedyLamarrGui;
}


class HedyLamarrGui : public QMainWindow
{
    Q_OBJECT
public:
    HedyLamarrGui(QWidget *parent = nullptr);
    ~HedyLamarrGui();

    std::atomic<bool> *isHedyLamarrStabilizing;
    std::atomic<bool> *isHedyLamarrConnected;

    void startStabilization(std::stringstream &ss); ///<
    void stopStabilization(std::stringstream &ss); ///<
    void connectToHedyLamarr(std::stringstream &ss);
    void disconnectFromHedyLamarr(std::stringstream &ss);
    std::shared_ptr<datacontainers::CentroidStabilization> centroidContainer = nullptr;

    //double updateRate; //Set to 1 sec, independent of camera exposure time
private:
    Ui::HedyLamarrGui *ui;
    QTcpSocket hedylamarr_socket;
    std::unique_ptr<std::thread> stabilizationLoop; ///<
    std::atomic<bool> stabilizationInProcess; ///<
    std::atomic<bool> stabilize;
    std::atomic<bool> isStabMoving;

    int updateRate = 2; //seconds
    datacontainers::HedyLamarrInfo HedyLamarrParams;
    void Stabilize(); ///<
    void moveNorth();
    void moveSouth();
    void moveWest();
    void moveEast();
    void setCurrentOffset();
    double NSoffset;
    double EWoffset;
    double stepSize;

    //Format variables for the values in the gui display.
    int positionPrecision = 4;
    char positionFormat = 'f';
    //Plot functions and variables
    void initPlot();
    //QVector<QCPGraphData> plotDataHTilt;
    //QVector<QCPGraphData> plotDataVTilt;
    //QVector<QCPGraphData> plotDataXDeviation;
    //QVector<QCPGraphData> plotDataYDeviation;

    QVector<QCPGraphData> plotData_hTilt;
    QVector<QCPGraphData> plotData_vTilt;
    QVector<QCPGraphData> plotData_hPix;
    QVector<QCPGraphData> plotData_vPix;
    double maxYRangePlot = 10.0; //
    int maxXRangePlot = 300; //5 min

    QString folderName;
    std::fstream stabilizationDataStream;
signals:
    void newCommand(QString command);
    void newCentroid();
    void newMessage(QString message, bool error = false);
    void updateStabilizationPlot();
public slots:
    void sendCommand(QString command);
    void displayMessage(QString message, bool error = false);
    void displayResponse(QString pos);
    void updateDisplay();
    void updatePlot();
    void SetStepSize();
};


#endif // HEDYLAMARRGUI_H

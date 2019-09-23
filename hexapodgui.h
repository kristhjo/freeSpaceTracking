#ifndef HEXAPOD_H
#define HEXAPOD_H
#include <iostream>
#include <QMainWindow>
#include <QWidget>
#include "PI_GCS2_DLL.h"
#include "datacontainers.h"
#include <opencv2/opencv.hpp>
#include <QVector>
namespace Ui {
class HexapodGui;
}

class KeyPressEater : public QObject
{
    Q_OBJECT
public:
    KeyPressEater(){}
    ~KeyPressEater(){}
    protected:
    inline bool eventFilter(QObject *obj, QEvent *event){
        if (event->type() == QEvent::KeyPress) {
            return true;
        } else {
            // standard event processing
            return QObject::eventFilter(obj, event);
    }
    }
};

class HexapodGui : public QMainWindow
{
    Q_OBJECT
public:
    HexapodGui(QWidget *parent = nullptr);
    ~HexapodGui();

    std::atomic<bool> *isHexapodStabilizing; ///< Flags that the hexapod is stabilizing. Points to the same boolean value in TrackingGui, such that a local change in this gui is registered in both cameragui and trackingui.
    std::atomic<bool> *isHexapodConnected;
    std::atomic<bool> *isManualControlActivated;

    void startStabilization(std::stringstream &ss); ///<
    void stopStabilization(std::stringstream &ss); ///<
    void connectToHexapod(std::stringstream &ss); ///< Connects to hexapod in the local network, with ip adress specified in the struct hexapodParams;
    void activateManualControl(std::stringstream &ss); ///<
    std::shared_ptr<datacontainers::CentroidStabilization> centroidContainer = nullptr;

    double updateRate; //Set to the camera exposure time.
private:
    Ui::HexapodGui *ui;
    KeyPressEater *keyPressEater = new KeyPressEater();   //Qt eventfilter for keyboard input. This is activated whenever manual control should be deactivated.

    int ID; ///< Identification number of connected hexapod
    double currentPos[6]; ///< Current coordinates relative to the active coordinate system.
    double currentAbsPos[6]; ///< Current coordinates relative to the ZERO coordinate system.
    double currentCoSys[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; ///< Current coordinates of origo in the active coordinate system relative the ZERO coordinate system.
    double velocity[6]; //Platform velocity. mm/s (mrad/s)
    bool referenceIfNeeded(); ///< Performs a reference procedure if the position of the hexpod axes is unknown to the hexapod.
    std::unique_ptr<std::thread> stabilizationLoop; ///<
    std::atomic<bool> stabilizationInProcess; ///<
    std::atomic<bool> stabilize;
    std::atomic<bool> isStabMoving;

    datacontainers::HexapodInfo hexapodParams;
    void MoveToPosition(double *dVal, bool moveRelative);
    void Stabilize(); ///<
    void pauseUntilHexapodStops(); ///< returns when hexapod motion is finished.
    std::string activeCoordinateSystem = "ZERO"; ///<activeCoordinateSystem has two possible settings in this implementation: ZERO and USERDEFINED
    //manual control functions
    int stepSize;
    void keyPressEvent(QKeyEvent *event); ///<overloaded qt function, interpreting keyboard input as hexapod motion commands.
    void setStepSize();
    //Format variables for the values in the gui display.
    int positionPrecision = 4; ///< Number of decimal digits of coordinates in the gui display
    char positionFormat = 'f'; ///< QString format of coordinates in the gui display.
    //Plot functions and variables
    void initPlot();
    QVector<QCPGraphData> plotDataHTilt;
    QVector<QCPGraphData> plotDataVTilt;
    QVector<QCPGraphData> plotDataXDeviation;
    QVector<QCPGraphData> plotDataYDeviation;
    double maxYRangePlot = 10.0; //
    int maxXRangePlot = 300; //5 min

    QString folderName;
    std::fstream stabilizationDataStream;
signals:
    void positionChanged();
    void coSysChanged();
    void newMessage(QString message, bool error = false);
    void updateStabilizationPlot();
    void newPositionMessage(QString positionString);
    //void displayLivePosition(double *position);
public slots:
    void displayMessage(QString message, bool error = false);
    void displayPosition(QString positionString);
    void updateCoSysDisplay();
    void updateDisplayPositions();
    void setUserDefinedCoSys();
    void setZeroCoSys();
    void moveToZero();
    void moveToDisplayPosition();
    void updatePlot();
};



#endif // HEXAPOD_H

#ifndef HEXAPOD_H
#define HEXAPOD_H
#include <iostream>
#include <QMainWindow>
#include <QWidget>
#include "PI_GCS2_DLL.h"
#include "datacontainers.h"
#include <opencv2/opencv.hpp>
#include <QVector>
#include <plotdisplay.h>

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

/********************************************//**
 *  Container for relevant parameters of the H-850.H2 Hexapod
 ***********************************************/
struct HexapodInfo
{
//range
double Xmax = 0.05;     //meter
double Xmin = -0.05;    //meter
double Ymax = 0.05;     //meter
double Ymin = -0.05;    //meter
double Zmax = 0.025;    //meter
double Zmin = -0.025;   //meter
//rotation in XY plane
double Wmax = 30;       //degree
double Wmin = -30;      //degree
//rotation in XZ plane
double Vmax = 15;       //degree
double Vmin = -15;      //degree
//rotation in ZY plane
double Umax = 15;       //degree
double Umin = -15;      //degree

//minimum increment
double dX = 0.3e-3;     //mm
double dY = 0.3e-3;     //mm
double dZ = 0.2e-3;     //mm
double dW = 5e-6;       //rad
double dV = 3e-6;       //rad
double dU = 3e-6;       //rad

//max velocity
double vX = 5e-4;       //m/s
double vY = 5e-4;       //m/s
double vZ = 5e-4;       //m/s
double vW = 6e-3;       //rad/s
double vV = 6e-3;       //rad/s
double vU = 6e-3;       //rad/s

QMap<QString, int> coAxisMap{{"X",0}, {"Y",1}, {"Z",2}, {"U",3}, {"V",4}, {"W",5}};
};

struct config_HexapodGui
{
//const char* ipadr;
QString ipadr;
int port = 50000;
//"PID" params
double integrationTime;
double deadTime;
double proportionalGain;
double maxMotion;
QString horizontalAxis;
QString verticalAxis;
int horizontalSign;
int verticalSign;
QString storage_location;
int horizontalAxisIndex;
int verticalAxisIndex;

//from trackingOptics
double pixToHex;
double pixel_size;
double magnification_telescope; 
double focalLength_eyepiece; 
double pixFOV_urad; 
double pixFOVdeg; 
double degToRad = 6.28/360;
double radToDeg = 360/6.28;
void update_pix_FoV(){
    pixFOV_urad = pixel_size/(focalLength_eyepiece*magnification_telescope)*1e6;
    pixFOVdeg = pixFOV_urad*radToDeg*1e-6; 
    pixToHex = pixFOVdeg;
    std::cout << pixToHex << std::endl;
}
};



struct stabilizationData{
    stabilizationData(){
        hexapod_horizontal = QSharedPointer<QCPGraphDataContainer>::create();
        hexapod_vertical = QSharedPointer<QCPGraphDataContainer>::create();
        centroid_horizontal = QSharedPointer<QCPGraphDataContainer>::create();
        centroid_vertical = QSharedPointer<QCPGraphDataContainer>::create();
    }

    QSharedPointer<QCPGraphDataContainer> hexapod_horizontal = nullptr;
    QSharedPointer<QCPGraphDataContainer> hexapod_vertical = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_horizontal = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_vertical = nullptr;
};

struct stabilizationPlots{
     stabilizationPlots():
         stabilization(plot(layoutPosition(0,0,1,1), "stabilizationPlot", labelContainer("", "[urad]", "stabilization", "", "pixels")))
     {}
     scatterplot stabilization;
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
    void loadConfiguration(QString configurationFilePath);
private:
    Ui::HexapodGui *ui;
    KeyPressEater *keyPressEater = new KeyPressEater();   //Qt eventfilter for keyboard input. This is activated whenever manual control should be deactivated.

    QString projectFolderPath;
    QString currentFolderPath;
    config_HexapodGui m_configurationSettings;
    HexapodInfo hexapodParams;
    void configureSettings();
    stabilizationData m_stabilizationData;
    stabilizationPlots m_stabilizationPlots;
    std::unique_ptr<plotDisplay> plotsWindow = nullptr;
    void setup_plots_window();
    void disableCommands(bool disable);
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

    void MoveToPosition(double *dVal, bool moveRelative);
    void Stabilize(); ///<
    void pauseUntilHexapodStops(); ///< returns when hexapod motion is finished.
    std::string activeCoordinateSystem = "ZERO"; ///<activeCoordinateSystem has two possible settings in this implementation: ZERO and USERDEFINED
    //manual control functions
    int stepSize;
    void keyPressEvent(QKeyEvent *event); ///<overloaded qt function, interpreting keyboard input as hexapod motion commands.
    void setStepSize();
    void setStepSizeSB();
    //Format variables for the values in the gui display.
    int positionPrecision = 4; ///< Number of decimal digits of coordinates in the gui display
    char positionFormat = 'f'; ///< QString format of coordinates in the gui display.
    //Plot functions and variables

    QString folderName;
    std::fstream stabilizationDataStream;
    std::fstream stabilizationDataStreamtest;

signals:
    void positionChanged();
    void coSysChanged();
    void newMessage(QString message, bool error = false);
    void updateStabilizationPlot();

    void newPositionMessage(QString positionString);
public slots:
    //display text
    void displayMessage(QString message, bool error = false);
    void displayPosition(QString positionString);
    //updates
    void updateCoSysDisplay();
    void updateDisplayPositions();
    void update_plotsWindow();
    //gui commands
    void setUserDefinedCoSys();
    void setZeroCoSys();
    void moveToZero();
    void moveToDisplayPosition();
    //"PID" updates
    void setDeadTime();
    void setIntegrationTime();
    void setMaxMotion();
    void setProportionalGain();
};



#endif // HEXAPOD_H

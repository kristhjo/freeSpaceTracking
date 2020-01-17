#ifndef TRACKINGGUI_H
#define TRACKINGGUI_H

#include <QMainWindow>
#include "cameragui.h"
#include "seeinggui.h"
#include "hexapodgui.h"
#include "datacontainers.h"
#include "hedylamarrgui.h"

namespace Ui {
class TrackingGui;
}

/********************************************//**
 * Main GUI. The TrackingGui class hosts the main body of the application.
 * Within this class, the camera parameters can be modified, and the interface with the cameragui, seeinggui and hexapodgui is established.
 * Communication between these GUIs must be arranged through this class.
 ***********************************************/

struct config_TrackingGui{
    int FrameRate;
    int ExposureTime;
    int Gain;
    int FrameHeight;
    int FrameWidth;
    int OffsetX;
    int OffsetY;
    int trackingThresh;
    int windowRadius;
    bool useWindowing;
    QString BaumerID;
};


class TrackingGui : public QMainWindow
{
    Q_OBJECT

public:
    explicit TrackingGui(QWidget *parent = nullptr);  ///< Constructor for TrackingGui. Initializes all GUI widgets.
    ~TrackingGui(); ///< Destructor for TrackingGui.

protected slots:

    void configureSettings();
    void loadConfiguration();

    void ConnectToCamera(); ///< Sets the chosen camera ID, and calls the connect() function of the cameragui.
    void StartCamera(); ///< Calls the Start() function of the cameragui.
    void StopCamera(); ///< Calls the Stop() function of the cameragui.
    void DisconnectFromCamera(); ///< Calls the disconnect() function of the cameragui.
    void SetExposureTime(); ///< Changes the exposure time of the BaumerCam upon interaction with the gui slider
    void SetExposureTimeSB(); ///< Changes the exposure time of the BaumerCam upon interaction with the gui spin box
    void SetGain(); ///< Changes the gain of the BaumerCam upon interaction with the gui spin box
    void SetWidth(); ///< Changes the image width of the BaumerCam upon interaction with the gui slider
    void SetWidthSB(); ///< Changes the image width of the BaumerCam upon interaction with the gui spin box
    void SetHeight(); ///< Changes the image height of the BaumerCam upon interaction with the gui slider
    void SetHeightSB(); ///< Changes the image height of the BaumerCam upon interaction with the gui spin box
    void SetYOffset(); ///< Changes the vertical offset of the BaumerCam upon interaction with the gui slider
    void SetYOffsetSB(); ///< Changes the vertical offset of the BaumerCam upon interaction with the gui spin box
    void SetXOffset(); ///< Changes the horizontal offset of the BaumerCam upon interaction with the gui slider
    void SetXOffsetSB(); ///< Changes the horizontaloffset of the BaumerCam upon interaction with the gui spin box
    void SetFrameRateSB();

    void StartTracking(); ///< Flags that CameraGui shall start calculating and displaying the image centroids
    void StopTracking(); ///< Flags that CameraGui shall stop calculating and displaying the image centroids
    void useWindowing(); ///< Flags that CameraGui shall calculate image centroids with the windowing algorithm.
    void setWindowRadius(); ///< Sets the radius the cameragui will use when calculating centroids with the windowing algorithm.
    void useThresholding(); ///< Flags that CameraGui shall calculate image centroids with the threshold algorithm.
    void setTrackingThreshold();  ///< Sets the intensity threshold the cameragui will use when calculating centroids with the threshold algorithm.

    void ConfigureSeeing();   ///< Starts an instance of the SeeingGui.
    void StartSeeingMeasurement(); ///< Starts a seeing measurement with the settings chosen in the SeeingGui. Requires the user to configure the measurement with ConfigureSeeing in advance.
    void StopSeeingMeasurement(); ///<
    void disableWidgets(bool disable); ///< If freeze is true, disables all widgets in the gui. If false, enables all widgets.

    void ConnectHexapod();  ///< Starts an instance of the HexapodGui, automatically initalizing the startup procedure of the hexapod.
    void StartStabilization();  ///< Flags that the hexapod stabilization shall start, and that cameragui shall start sharing image centroids with hexapodgui.
    void StopStabilization(); ///< Flags that hexapod stabilization shall stop.
    void ManuallyControlHexapod(); ///< Initializes manual control of the hexapod through the keyboard.

    void StartHedyLamarrStabilization();
    void StopHedyLamarrStabilization();
    void ConnectHedyLamarr();
    void DisconnectHedyLamarr();
private:
    Ui::TrackingGui *ui;
    config_TrackingGui m_configurationSettings;
    //datacontainers::configurationSettings m_configurationSettings;
    //datacontainers::configurationSettings m_configurationSettings_main;
    QString m_configurationFilePath;
    QString m_projectFolderPath;

    std::atomic<bool> isCameraConnected; ///< Flags that connection with the Baumer camera has been established.
    std::atomic<bool> isHexapodConnected; ///< Flags that connection with the hexapod has been established.
    std::atomic<bool> isCameraRunning; ///< Flags that the image acquisition loop of the Baumer cam has started. Is shared with cameragui.
    std::atomic<bool> isCameraTracking; ///< Flags that centroid tracking is activated. Is shared with cameragui.
    std::atomic<bool> isMeasuringSeeing; ///< Flags that a seeing measurement is in progress. Is shared with cameragui.
    std::atomic<bool> isHexapodStabilizing; ///< Flags that the hexapod is actively stabilizing.
    std::atomic<bool> isHedyLamarrConnected;
    std::atomic<bool> isHedyLamarrStabilizing; ///< Flags that the hedy lamarr telescope is actively stabilizing.

    std::unique_ptr<HexapodGui> pm_hexapod = nullptr; ///< is assigned an instance of HexapodGui.
    std::unique_ptr<CameraGui> pm_Camera = nullptr; ///< is assigned an instance of CameraGui.
    std::unique_ptr<SeeingGui> pm_seeing = nullptr; ///< is assigned an instance of SeeingGui.
    std::unique_ptr<HedyLamarrGui> pm_hedylamarr = nullptr; ///< is assigned an instance of HedyLamarrGui.

    std::shared_ptr<datacontainers::ImageContainer> m_imageContainer = nullptr; ///< Contains images shared between cameragui and seeinggui
    std::shared_ptr<datacontainers::CentroidStabilization> centroidContainer = nullptr; ///< Contains centroids shared between cameragui and hexapodgui
    std::shared_ptr<datacontainers::CentroidStabilization> centroidContainerHedy = nullptr; ///< Contains centroids shared between cameragui and hedylamarrgui
};

#endif // TRACKINGGUI_H

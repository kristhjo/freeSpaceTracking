#ifndef CAMERAGUI_H
#define CAMERAGUI_H

#include<QObject>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <string>
#include <ctime>
#include <mutex>
#include <bits/stdc++.h>
#include "bgapi2_genicam/bgapi2_genicam.hpp"
#include <opencv2/opencv.hpp>
#include "datacontainers.h"
using namespace BGAPI2;

/********************************************//**
 * Class handling the interface with the BaumerCam. CameraGui interfaces with the baumercam through integration with their Baumer GAPI SDK software.
 * Cameragui also handles the image display through the matrix structures of the opencv library.
 * Tracking through image centroids (center-of-intensity) are calculated real-time,
 * and distributed to the hexapod stabilization algorithm or seeing measurement algorithm upon the activation of the corresponding flags (isTracking, isHexapodStabilizing, isMeasuringSeeing).
 ***********************************************/
class CameraGui: public QObject
{
    Q_OBJECT

public:
    explicit CameraGui(QObject *parent = nullptr); ///< Initializes a CameraGui object with a CamInfo struct from the datacontainer header.
    ~CameraGui();

    void Connect(std::stringstream &ss ); ///< Establishes ethernet connection with the chosen BaumerCam (IQOQI, Bisamberg).
    void Start(std::stringstream &ss);  ///< Starts acquisition and display of the images from the camera.
    void Stop(std::stringstream &ss); ///< Stops acquisition and display of camera images.
    void Disconnect(std::stringstream &ss); ///< Terminates the connection with the BaumerCam.
    void printCamInfo(std::stringstream &ss); ///< Prints the following camera settings: Gain, Exposure time, image height and width, horizontal and vertical offset.
    void updateCamParameters(); ///< Updates camera settings whenever a value of CamInfo is changed in the main gui.

    datacontainers::CamInfo m_CamInfo; ///< Struct containing all the relevant settings of the Baumer Cam.
    datacontainers::TrackingParameters m_TrackingParameters; ///< struct containing all the relevant settings for centroid tracking. This struct depends on the optical setup.
    cv::Point currentCentroid; ///< last calculated image centroid.

    std::atomic<bool> newCamParameter; ///< Flags whenever the image acquisition loop needs to update the camera settings.
    std::atomic<bool> *isCameraConnected = nullptr;
    std::atomic<bool> *isCameraTracking = nullptr; ///< Flags that the image acquisition loop shall start calculating the centroid of each image.
    std::atomic<bool> *isCameraRunning = nullptr; ///< flags if the camera is acquiring images.
    std::atomic<bool> *isHexapodStabilizing = nullptr; ///< Flags that the hexapod stabilization has started, so that the centroidContainer (which is shared with hexapodgui) is continuously fed with the current centroid.
    std::atomic<bool> *isMeasuringSeeing = nullptr; ///< Flags that a seeing measurement has started, such that images is fed to m_imageContainer (which is shared with seeinggui).
    std::atomic<bool> *isHedyLamarrStabilizing = nullptr; ///< Flags that the hedylamarr stabilization has started, so that the centroidContainer (which is shared with hedylamarrgui) is continuously fed with the current centroid.
    ///
    std::shared_ptr<datacontainers::ImageContainer> m_imageContainer = nullptr; ///< Image container shared between cameragui and seeinggui.
    std::shared_ptr<datacontainers::CentroidStabilization> centroidContainer = nullptr; ///< Centroid container shared between cameragui and hexapodgui.
private:
    std::atomic<bool> isPictureInProcess; ///< flags if the acquisition loop Run() is still processing a picture.
    std::unique_ptr<std::thread> m_CamThread; ///< Internal thread created within the Start() function. m_CamThread runs the continuous image acquisition through Run().
    std::atomic<bool> acquireImages; ///< flags if camera shall acquire images.
    ///Member Variables for the Baumer GAPI
    BGAPI2::SystemList * pm_systemList = nullptr;
    BGAPI2::System * pm_pSystem = nullptr;
    BGAPI2::String m_sSystemID;
    //---------Interface---------------------------
    BGAPI2::InterfaceList * pm_interfaceList ;
    BGAPI2::Interface * pm_pInterface ;
    BGAPI2::String m_sInterfaceID;
    //---------Device------------------------------
    BGAPI2::DeviceList * pm_deviceList;
    BGAPI2::Device * pm_pDevice;
    BGAPI2::String m_sDeviceID;
    //---------DataStream------------------------------
    BGAPI2::DataStreamList * pm_datastreamList;
    BGAPI2::DataStream * pm_pDataStream ;
    BGAPI2::String m_sDataStreamID;
    //---------Cam Buffer------------------------------
    BGAPI2::BufferList * pm_bufferList;
    BGAPI2::Buffer * pm_pBuffer;
    //---------Image Processor------------------------
    BGAPI2::ImageProcessor * pm_imgProcessor;
    BGAPI2::Image * pm_pImage;
    BGAPI2::Image * pm_pTransformImage ;


    void Run(); ///< Continuous loop handling the image acquisition and display, centroid calculations and uploads to m_imageContainer and centroidContainer. Terminates when stop() is called.
};

#endif // CAMERAGUI_H

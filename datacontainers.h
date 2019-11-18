#ifndef DATACONTAINERS_H
#define DATACONTAINERS_H
#include <iostream>
#include <mutex>
#include <queue>
#include <ctime>
#include <math.h>
#include <algorithm>
#include <QVector>
#include <QDateTime>
#include <qcustomplot.h>
#include <opencv2/opencv.hpp>
namespace datacontainers {

struct configurationSettings_seeingGui{
    configurationSettings_seeingGui(){
        store_images = false;
        debug = false;
        use_threshold = false;
        use_windowing = false;
        sample_size = 50;
        threshold = 20;
        window_radius=1;
        wavelength = 532; //nanometer
        storage_location = "/home/domi/Desktop/seeingTests";
        measurement_type = "Gaussian fit";
        apertureDiameter = 0.254;
        DIMM_apertureDiameter = 0.026;
        DIMM_baseline = 0.16;
        pixel_size = 5.5e-6;
        magnification_telescope = 1;
        focalLength_eyepiece = 1;
        update_DIMM_coefficients();
        update_pix_FoV();
    }
    int frameRate;
    int exposureTime;
    bool store_images;
    bool debug;
    bool use_threshold;
    bool use_windowing;
    int sample_size;
    int threshold;
    int window_radius;
    double wavelength;

    QString storage_location;
    QString measurement_type;    
    QDir baseDirectory;

    double magnification_telescope;
    double focalLength_eyepiece;
    double apertureDiameter;
    double airy_zeros_pix[20];
    double pixel_size;
    double DIMM_apertureDiameter;
    double DIMM_baseline;
    double DIMM_airy_zeros_pix[20];

    double pixFOV_urad;
    double pixFOV;
    double K_l;
    double K_t;
    void update_airy_zeros(){
        double AiryMinRatio = focalLength_eyepiece*magnification_telescope*(wavelength/apertureDiameter)/pixel_size;
        double DIMM_AiryMinRatio = focalLength_eyepiece*magnification_telescope*(wavelength/DIMM_apertureDiameter)/pixel_size;
        for (int i = 0; i<20;i++){
            airy_zeros_pix[i] = (i+1.22)*AiryMinRatio;
            DIMM_airy_zeros_pix[i] = (i+1.22)*DIMM_AiryMinRatio;
        }
    }
    void update_DIMM_coefficients(){
        K_t = 0.364*( 1-0.798 * std::pow((DIMM_baseline/DIMM_apertureDiameter), -1/3) - 0.018 * std::pow((DIMM_baseline/DIMM_apertureDiameter),-7./3.));
        K_l = 0.364*( 1-0.532 * std::pow((DIMM_baseline/DIMM_apertureDiameter), -1/3) - 0.024 * std::pow((DIMM_baseline/DIMM_apertureDiameter),-7./3.));
    }
    void update_pix_FoV(){
        pixFOV = pixel_size/(focalLength_eyepiece*magnification_telescope);
        pixFOV_urad = pixFOV*1e6;
    }
};



struct configurationSettings{
    configurationSettings(){
        FrameRate = 10;
        ExposureTime = 1000;
        Gain = 1;
        FrameHeight = 60;
        FrameWidth = 30;
        OffsetX = 390;
        OffsetY = 400;
        BaumerID = "devicemodul00_06_be_00_8f_92";
        trackingThresh = 20;
        windowRadius = 1;
        default_storageLoc_seeing = "home/domi/Desktop/seeingTests/ ";
        Hexapod_ipadr   = "193.170.58.178";
        config_seeingGui = configurationSettings_seeingGui();

    }
    //Initial camera settings
    std::string BaumerID;
    int FrameRate;
    int ExposureTime;
    int Gain;
    int FrameHeight;
    int FrameWidth;
    int OffsetX;
    int OffsetY;

    //Initial tracking settings
    int trackingThresh; //brightness threshold 0-255
    int windowRadius;//pixels from center
    bool useWindowing = false;

    //Initial seeing seetings
    configurationSettings_seeingGui config_seeingGui;
    std::string default_storageLoc_seeing;
    double DIMM_mask_apertureDiameter = 0; //does not exist at this time
    double DIMM_mask_baseline = 0; //does not exist at this time
    int DIMM_airy_zeros_pix[20];

    //Characteristics of the optical setup
    double pixelFOV = 5.5*1e-6/1.140; //radians
    double apertureDiameter = 0.5; // ? meters
    double focalLength_eyepiece = 0.05;
    double magnification_telescope = 1;
    int airy_zeros_pix [20]; //location of the 20 first airy minimas at the camera in pixels
    //Initial hexapod settings
    const char* Hexapod_ipadr;
    std::string default_storageLoc_stabilization = "home/domi/Desktop/trackingLogs/ ";

};



/********************************************//**
 *  Container for default settings of the IQOQI side setup
 ***********************************************/

struct IQOQI_seeingConfig : configurationSettings_seeingGui{
    IQOQI_seeingConfig(){
        store_images = true;
        debug = true;
        use_threshold = true;
        use_windowing = false;
        sample_size = 5;
        threshold = 10;
        window_radius = 1;
        wavelength = 532e-9;
        storage_location = "/home/kristian/Desktop/seeingTests";
        measurement_type = "Gaussian fit";
        magnification_telescope = 1;
        focalLength_eyepiece = 1.140;
        apertureDiameter = 0.4;
        update_pix_FoV();
        update_airy_zeros();
    }
};
struct IQOQI_setup : configurationSettings
{
    IQOQI_setup(){
        BaumerID = "devicemodul00_06_be_00_8f_92";
        FrameRate = 5;
        ExposureTime = 50;
        Gain = 1;
        FrameHeight = 45;
        FrameWidth = 22;
        OffsetX = 234;
        OffsetY = 307;
        pixelFOV = 5.5*1e-6/1.140; //radians
        Hexapod_ipadr   = "193.170.58.178";
        default_storageLoc_seeing = "home/kristian/Desktop/seeingTests/ ";
        config_seeingGui = IQOQI_seeingConfig();        
    }
};


/********************************************//**
 *  Container for default settings of the Bisamberg side setup
 ***********************************************/

struct BISAM_seeingConfig : configurationSettings_seeingGui{
    BISAM_seeingConfig(){
        store_images = true;
        debug = true;
        use_threshold = true;
        use_windowing = false;
        sample_size = 50;
        threshold = 10;
        window_radius = 3;
        wavelength = 532e-9;
        storage_location = "/home/bisamlinux/Desktop/seeingTests";
        measurement_type = "Gaussian fit";
        magnification_telescope = 2.032/0.05;
        focalLength_eyepiece = 0.05;
        DIMM_baseline = 0.16;
        DIMM_apertureDiameter = 0.026;

        update_DIMM_coefficients();
        update_airy_zeros();
        update_pix_FoV();
    }
};

struct BISAM_setup  : configurationSettings
{
    BISAM_setup(){
        BaumerID  = "devicemodul00_06_be_00_9d_56";
        FrameRate = 10;
        ExposureTime = 5000;
        Gain = 1;
        FrameHeight = 60;
        FrameWidth = 30;
        OffsetX = 390;
        OffsetY = 400;

        trackingThresh = 20; //brightness threshold 0-255
        windowRadius = 1; //airy min from center

        pixelFOV = 5.5*1e-6/(0.05*2.032/0.05); //radians

        Hexapod_ipadr   = "193.170.58.178";
        default_storageLoc_seeing = "home/bisamLinux/Desktop/seeing/ ";

        DIMM_mask_apertureDiameter = 0.026;
        DIMM_mask_baseline = 0.16;

        Hexapod_ipadr   = "193.170.58.184";
        config_seeingGui = BISAM_seeingConfig();
    }
};





/********************************************//**
 *  Container for relevant parameters of the Hedy Lamarr Telescope
 ***********************************************/
struct HedyLamarrInfo
{
double focalLength = 1.140; //meter
double pixFieldOfView = 5.5e-6/focalLength; //radians
double radToArcSec = 206264.81;
//conversion factor: pixel distance of Baumer camera to hedy lamarr motion
double pixToHedyLamarr = pixFieldOfView*radToArcSec; //rough calculation, pixel to degrees rotation
};

/********************************************//**
 *  Container for relevant parameters of the receiver optics
 ***********************************************/
struct receiverOpticsInfo
{
  double f1 = 2.032; // meade telescope
  double f2 = 0.05; // achromat collimator lens
  double f3 = 0.05; // achromat focusing lens
  double magnification = f1/f2; //magnification of collimation setup
  double pixSize = 5.5e-6;
  double pixFoVrad = pixSize/(f3*magnification);
  double pixFoVdeg = pixFoVrad*360/(6.28);//0.00015516
};

/********************************************//**
 *  Container for relevant parameters of the H-850.H2 Hexapod
 ***********************************************/
struct HexapodInfo
{
//const char* ipadr   = "193.170.58.178"; Hexapod IQOQI
const char* ipadr   = "193.170.58.184"; //Hexapod Bisamberg
int port = 50000;
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

//conversion factor: pixel distance to hexapod motion
receiverOpticsInfo receiverOptics;
double pixToHex = receiverOptics.pixFoVdeg;
//double pixToHex = 0.0001937*0.5;//*7.5/5; //rough calculation, pixel to degrees rotation
};

/********************************************//**
 *  Container for thread-safe centroid sharing between cameragui and hexapodgui
 ***********************************************/
struct CentroidStabilization
{
   cv::Point homeCentroid;
   double homeX;
   double homeY;
   cv::Point currentCentroid;
   double meanCentroidX;
   double meanCentroidY;
   std::vector<cv::Point> centroidHistory;
   std::mutex lockCentroidHistory;
   std::atomic<bool> newCentroid;
   void addCentroid(cv::Point centroid){
       std::lock_guard<std::mutex> guard(lockCentroidHistory);
       centroidHistory.push_back(centroid);
   }
   void emptyHistroy(){
       std::lock_guard<std::mutex> guard(lockCentroidHistory);
       centroidHistory.clear();
   }

   void updateMeanCentroid(){
       std::lock_guard<std::mutex> guard(lockCentroidHistory);
       double meanX = 0;
       double meanY = 0;
       for (unsigned int i = 0; i < centroidHistory.size(); i++){
           meanX+= centroidHistory.at(i).x;
           meanY += centroidHistory.at(i).y;
       }
       meanCentroidX = meanX/centroidHistory.size();
       meanCentroidY = meanY/centroidHistory.size();
   }
};

/********************************************//**
 *  Container for relevant parameters of Baumer VLG-22M camera
 ***********************************************/
struct CamInfo
{
    CamInfo()
    {
        DeviceNr = "";
        ExposureMax = 1000000;
        ExposureMin = 15;
        ExposureActual = 1790;
        GainMax = 0;
        GainMin = 0;
        GainActual = 1;
        BlackLevelMax = 0;
        BlackLevelMin = 0;
        BlackLevelActual = 0;
        FrameHeightMax=1084;
        FrameWidthMax=2044;
        FrameHeight=1084;
        FrameWidth=2044;
        OffsetX=0;
        OffsetY=0;
        frameRate=10.0;
    }
    std::string DeviceNr;
    double FrameRate;
    double ExposureMax;
    double ExposureMin;
    double ExposureActual;
    int GainMax;
    int GainMin;
    int GainActual;
    int BlackLevelMax;
    int BlackLevelMin;
    int BlackLevelActual;
    int FrameHeightMax;
    int FrameWidthMax;
    int FrameHeight;
    int FrameWidth;
    int OffsetX;
    int OffsetY;
    double frameRate;
    std::string BaumerID;
    //std::string BaumerIQOQI = "devicemodul00_06_be_00_8f_92";
    //std::string BaumerBisamberg = "devicemodul00_06_be_00_9d_56";
};

/********************************************//**
 *  Container for relevant parameters of the centroid tracking algorithm
 ***********************************************/
struct TrackingParameters
{
    int TrackingThresh = 10; //brightness threshold 0-255
    int WindowRadius = 1; //pixels from center
    std::atomic<bool> useWindowing;
};

/********************************************//**
 *  Container for relevant parameters of the seeing measurement module
 ***********************************************/
struct seeingParameters
{
    seeingParameters(){
        double AiryMinRatio = receiverOptics.f3*receiverOptics.magnification*(wavelength/maskApertureDiameter)/receiverOptics.pixSize;
        for (int i = 0; i<20;i++){
            pxAiryZeros[i] = int( (i+1.22)*AiryMinRatio +0.5 );
         }
    }
    int intensityThreshold = 1;
    double exposureTime;
    double frameRate;
    double pixFov;
    receiverOpticsInfo receiverOptics;
    double iqoqiPixFov = receiverOptics.pixSize/1.140;
    double bisamPixFov = receiverOptics.pixFoVrad;
    //meters
    double maskApertureDiameter = 0.026; //DIMM mask
    double maskApertureSeparation = 0.16; //DIMM mask
    double wavelength = 532e-9; //Green LEDs
    //double focalLength = receiverOptics.f3;// final focusing lens
    double pxWidth = receiverOptics.pixSize; //Baumer cam
    double pxHeight = receiverOptics.pixSize; //Baumer cam
    //double magnification = receiverOptics.magnification;
    int pxAiryZeros [20];//location of airy minimas in the image plane in pixels.
    int pxWindowRadius = 10;
    //int pxWindowRadius = 10; //radius of crop window used in windowing algorithm. Is set through the seeinggui.
    //parameters for DIMM calculations
    double K_l = 0.364*( 1-0.532 * std::pow((maskApertureSeparation/maskApertureDiameter), -1/3) - 0.024 * std::pow((maskApertureSeparation/maskApertureDiameter),-7./3.));
    double K_t = 0.364*( 1-0.798 * std::pow((maskApertureSeparation/maskApertureDiameter), -1/3) - 0.018 * std::pow((maskApertureSeparation/maskApertureDiameter),-7./3.));
};


/********************************************//**
 *  Container for thread-safe image sharing between cameragui and seeinggui
 ***********************************************/
struct ImageContainer
{
    std::mutex lockQueue;
    std::queue<cv::Mat> imgQueue;

    int sampleSize = 1;
    int imgCounter = 1;

    time_t startTime;
    time_t endTime;
    void addImg(cv::Mat img){
        std::lock_guard<std::mutex> guard(lockQueue);
        imgQueue.push(img);
    }
    void removeFirstImg(){
        std::lock_guard<std::mutex> guard(lockQueue);
        imgQueue.pop();
    }

    void clearQueue(){
        std::lock_guard<std::mutex> guard(lockQueue);
        imgQueue = {};
    }
};


/********************************************//**
 *  Container for spot separations of single DIMM sample, containing functions for returning the average and the variance of the sample.
 ***********************************************/
struct DIMMsample
{
    std::vector<cv::Point> spotSeparation; //in pixels

    inline double average_x(){
        double sum = 0.0;
        for (unsigned int i = 0; i < spotSeparation.size(); i++){
            sum+= spotSeparation.at(i).x;
        }
        return (sum/spotSeparation.size());
    }
    inline double average_y(){
        double sum = 0.0;
        for (unsigned int i = 0; i < spotSeparation.size(); i++){
            sum+= spotSeparation.at(i).y;
        }
        return (sum/spotSeparation.size());
    }
    inline double variance_x(){
        double sum = 0.0;
        double avg = average_x();
        for ( unsigned int j =0; j < spotSeparation.size(); j++){
            sum+= std::pow((spotSeparation[j].x - avg),2);
        }
        return sum/(spotSeparation.size() - 1); //divide by n-1 since this is an estimator of the real variance.
    }
    inline double variance_y(){
        double sum = 0.0;
        double avg = average_y();
        for ( unsigned int j =0; j < spotSeparation.size(); j++)        {
            sum += std::pow((spotSeparation[j].y - avg),2);
        }
        return sum/(spotSeparation.size() - 1); //divide by n-1 since this is an estimator of the real variance.
    }
};

struct gaussianFitParams{
    double intensitymax;
    double var_x;
    double var_y;
    double center_x;
    double center_y;
    double sigma_cov;
    double numSaturatedPixels;
};

struct GaussSample{
    gaussianFitParams fitParams;
    cv::Mat gaussImg;
    inline double FWHM_x(){
        return pow(fitParams.var_x,0.5)*2.355;
    }
    inline double FWHM_y(){
        return pow(fitParams.var_y,0.5)*2.355;
    }
};

/********************************************//**
 *  Container for seeing data collected continuously through the measurement
 ***********************************************/

struct seeingValues
{
    QVector<double> seeing;
    QVector<double> seeing_x;
    QVector<double> seeing_y;
    QVector<double> fried;
    QVector<double> fried_x;
    QVector<double> fried_y;
    QVector<time_t> timestamps;
    QVector<QCPGraphData> friedData;
    QVector<QCPGraphData> friedData_x;
    QVector<QCPGraphData> friedData_y;
    QVector<QCPGraphData> seeingData;
    QVector<double> plotTimes;
    QVector<QCPGraphData> avgFriedPlot;
    QVector<QCPGraphData> avgSeeingPlot;
    double minFried(){
        return *std::min_element(fried.constBegin(), fried.constEnd());
    }
    double maxFried(){
        return *std::max_element(fried.constBegin(), fried.constEnd());
    }
    double minSeeing(){
        return *std::min_element(seeing.constBegin(), seeing.constEnd());
    }
    double maxSeeing(){
        return *std::max_element(seeing.constBegin(), seeing.constEnd());
    }
    double meanFried(){
        return std::accumulate(fried.begin(), fried.end(), .0) / fried.size();;
    }
    double meanFried_x(){
        return std::accumulate(fried_x.begin(), fried_x.end(), .0) / fried_x.size();;
    }
    double meanFried_y(){
        return std::accumulate(fried_y.begin(), fried_y.end(), .0) / fried_y.size();;
    }
    double maxFried_x(){
        return *std::max_element(fried_x.constBegin(), fried_x.constEnd());
    }
    double maxFried_y(){
        return *std::max_element(fried_y.constBegin(), fried_y.constEnd());
    }
    double minFried_x(){
        return *std::min_element(fried_x.constBegin(), fried_x.constEnd());
    }
    double minFried_y(){
        return *std::min_element(fried_y.constBegin(), fried_y.constEnd());
    }

    double meanSeeing(){
        return std::accumulate(seeing.begin(), seeing.end(), .0) / seeing.size();;
    }
};

}
#endif // DATACONTAINERS_H

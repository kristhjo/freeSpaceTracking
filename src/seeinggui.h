#ifndef SEEINGGUI_H
#define SEEINGGUI_H
#include <opencv2/opencv.hpp>
#include <iostream>
#include <QMainWindow>
#include <QWidget>
#include <QtCharts>
#include <qcustomplot.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <math.h>
#include "imageprocessing.h"
#include "datacontainers.h"
#include "plotdisplay.h"
namespace Ui{
class   SeeingGui;
}

/********************************************//**
 * SeeingGui is an interface for configuring and performing a seeing measurement.
 * The class is implemented with the DIMM measurement method, and assumes that the two spots from the DIMM measurement will locate in their respective left-right halves of the image.
 ***********************************************/

struct seeingData{
    seeingData(){
        fried = QSharedPointer<QCPGraphDataContainer>::create();
        avgFried = QSharedPointer<QCPGraphDataContainer>::create();
        seeing = QSharedPointer<QCPGraphDataContainer>::create();
        avgSeeing = QSharedPointer<QCPGraphDataContainer>::create();
    }
    seeingData(QString measurement_type, bool debug = true){
        fried = QSharedPointer<QCPGraphDataContainer>::create();
        seeing = QSharedPointer<QCPGraphDataContainer>::create();
        if (debug){
            centroid_x = QSharedPointer<QCPGraphDataContainer>::create();
            centroid_y = QSharedPointer<QCPGraphDataContainer>::create();
            saturation = QSharedPointer<QCPBarsDataContainer>::create();
            saturation->set(QVector<QCPBarsData>(256, QCPBarsData(0.0,0.0)));
            for(int i = 0; i< 256; i++){
                (saturation.data()->begin()+i)->key = i;
            }
            strehlData_Gauss = QSharedPointer<QCPGraphDataContainer>::create();
            if (measurement_type == "DIMM"){
                std::cout << "init dimm" << std::endl;
                centroid_x2 = QSharedPointer<QCPGraphDataContainer>::create();
                centroid_y2 = QSharedPointer<QCPGraphDataContainer>::create();
                strehlData_DIMM1 = QSharedPointer<QCPBarsDataContainer>::create();
                strehlData_DIMM2 = QSharedPointer<QCPBarsDataContainer>::create();
                horizontalVarData = QSharedPointer<QCPBarsDataContainer>::create();
                verticalVarData = QSharedPointer<QCPBarsDataContainer>::create();
            }
        }
    }
    QSharedPointer<QCPGraphDataContainer> fried = nullptr;
    QSharedPointer<QCPGraphDataContainer> seeing = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_x = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_y = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_x2 = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_y2 = nullptr;
    QSharedPointer<QCPBarsDataContainer> saturation = nullptr;
    QSharedPointer<QCPBarsDataContainer> strehlData_DIMM1 = nullptr;
    QSharedPointer<QCPBarsDataContainer> strehlData_DIMM2 = nullptr;
    QSharedPointer<QCPGraphDataContainer> strehlData_Gauss = nullptr;
    QSharedPointer<QCPBarsDataContainer> horizontalVarData = nullptr;
    QSharedPointer<QCPBarsDataContainer> verticalVarData = nullptr;
    QSharedPointer<QCPGraphDataContainer> avgFried = nullptr;
    QSharedPointer<QCPGraphDataContainer> avgSeeing = nullptr;
};

struct seeingPlots{
     seeingPlots():
         seeing(plot(layoutPosition(0,0,1,2), "seeingPlot", labelContainer("", "[urad]", "seeing"))),
         fried(plot(layoutPosition(1,0,1,2), "friedPlot", labelContainer("", "[cm]", "fried"))),
         centroid(plot(layoutPosition(2,0,1,1), "centroidPlot", labelContainer("", "", "centroid motion"))),
         saturation(plot(layoutPosition(2,1,1,1), "saturationPlot", labelContainer("intensity", "counts", "image saturation"))),
         cm_image(plot(layoutPosition(0,2,1,1), "imageColormap", labelContainer("x", "y", "image"))),
         cm_fit(plot(layoutPosition(1,2,1,1), "fitColormap", labelContainer("x", "y", "fit"))),
         cm_residue(plot(layoutPosition(2,2,1,1), "residueColormap", labelContainer("x", "y", "residue"))),
         strehl(plot(layoutPosition(0,2,1,1), "strehl ratio", labelContainer("strehl", "images", "Strehl"))),
         vVar(plot(layoutPosition(1,2,1,1), "vVar", labelContainer("distance", "count", "longitudinal variance"))),
         hVar(plot(layoutPosition(2,2,1,1), "hVar", labelContainer("distance", "count", "horizontal variance")))
     {}
     scatterplot seeing;
     scatterplot fried;
     scatterplot centroid;
     histogram saturation;
     //Gauss specific
     colorMap cm_image;
     colorMap cm_fit;
     colorMap cm_residue;
     //DIMM specific
     histogram strehl;
     histogram vVar;
     histogram hVar;
};

struct config_seeingGui{
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

    int pixel_saturation_cutOff;
    double strehl_cutOff;
    QString storage_location;
    QString measurement_type;
    QDir baseDirectory;
    QString measurement_folder_name;

    //optics
    double magnification_telescope;
    double focalLength_eyepiece;
    double apertureDiameter;
    double pixel_size;
    double DIMM_apertureDiameter;
    double DIMM_baseline;

    double airy_zeros_pix[20];
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

struct outputFiles{
    QDir baseDir;
    QString currentSubFolderName; ///< Holds the file path to the current subfolder in basedirectory. Each subfolder corresponds to a sample size, and hosts the images of the sample.
    QString currentFolderPath;
    QString projectFolderPath;
    QFile parameterFile;
    QTextStream writeParametersToFile;
    QFile resultsFile;
    QTextStream writeResults;
};


class SeeingGui : public QMainWindow
{
    Q_OBJECT
public:
    SeeingGui(QWidget *parent = nullptr);
    ~SeeingGui();

    std::atomic<bool> *isMeasuringSeeing = nullptr; ///< When set to true, cameragui starts uploading images to m_imageContainer.
    std::shared_ptr<datacontainers::ImageContainer> m_imageContainer = nullptr; ///< Image container for one DIMM sample. After each fried parameter is calculated, the container is emptied and filled again by cameragui.

    void startMeasurement(); ///< Initiates a seeing measurement with the measurement settings in the gui.
    void stopMeasurement(); ///< Terminates the currently running seeing measurement. Handles the safe termination of the DIMM_thread, and stores the results.
    void loadConfiguration(QString configurationFile);
    void configureSettings();
    void setCameraSettings(int frameRate, int exposureTime);

private:
    Ui::SeeingGui *ui;
    config_seeingGui m_configurationSettings;

    seeingPlots plots;
    std::unique_ptr<plotDisplay> plotsWindow = nullptr;
    seeingData m_seeingData;// = nullptr;

    datacontainers::GaussSample m_GaussSample;
    datacontainers::DIMMsample m_DIMMsample; ///< Contains the spotseparations of a DIMM sample, as well as functions for calculating the average and variance of the sample.

    std::unique_ptr<std::thread> measurement_thread = nullptr;
    std::atomic<bool> isProcessing; ///< Flags that the real-time processing loop is active.

    outputFiles output;
    time_t startOfMeasurement; ///< Timestamp at the start of the measurement

    QString currentSubFolderName; ///< Holds the file path to the current subfolder in basedirectory. Each subfolder corresponds to a sample size, and hosts the images of the sample.
    QString currentFolderPath;
    QString projectFolderPath;

    QFile parameterFile;
    QTextStream writeParametersToFile;
    QFile resultsFile;
    QTextStream writeResults;

    void DIMM(); ///< Handles the real-time processing of images in m_imageContainer. DIMM() calculates the spotseparation of each image in m_imagecontainer and subsequently the fried parameter and seeing value when the sample size is reached. It also handles the storage of data according to the setup parameters, and continuously updates the gui display. DIMM() is run in a separate thread and terminates when stopMeasurement() is called.
    void Gaussian();
    void setup_plots_window();
    void setMeasurementSettings(); ///< Reads the measurement settings chosen in the gui.

    double getFriedParameter(); ///< Calculates the Fried parameter of m_DIMMsample.
    double getFriedParameter_x(); ///< Calculates the horizontal Fried parameter of m_DIMMsample.
    double getFriedParameter_y(); ///< Calculates the vertical Fried parameter of m_DIMMsample.
    double getSeeingFromFried(double FriedParameter, double wavelength); ///< Converts a Fried parameter to a seeing value.
    double getFriedFromSeeing(double SeeingParameter, double wavelength); ///< Converts a Seeing value into a Fried parameter
    double getAverage(QCPGraphDataContainer graph);

    void createParameterFile(); ///< Creates text file where the current seeing measurement parameters are written.
    void writeResultsToFile(); ///< Writes the results of the measurement to a text file.
    QString timestampToFolderName(time_t timestamp); ///< Formats a more readable folder name from the current unix timestamp.
    void setSecPerDataPoint(); ///< Displays the length in seconds of each measurement interval. I.e. the sample size times the exposure time of the camera.

private slots:
    void displayMessage(QString message, bool error = false); ///< Displays a message in the Gui. If error is true, an error message is printed to stdout through qDebug().
    void debugGaussian(const cv::Mat image);
    void setWindowRadius();
    void updateGaussPlots();
    void updateDIMMPlots();
signals:
    void newSeeingValues(); ///< Signals that plots and display values in the gui are to be updated.
    void newDIMMValues();
    void debugDIMMSample(datacontainers::DIMMsample     sample);
    void newMessage(QString message, bool error = false); ///< Emits a signal connected to displayMessage. Is needed within the DIMM_thread, as only the main thread can interact with the GUI.
    void debugGaussianSeeing(const cv::Mat &image);
};

#endif // SEEINGGUI_H

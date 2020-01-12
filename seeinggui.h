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
            centroid_x = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);
            centroid_y = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);
            saturation = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
            saturation->set(QVector<QCPBarsData>(256, QCPBarsData(0.0,0.0)));
            for(int i = 0; i< 256; i++){
                (saturation.data()->begin()+i)->key = i;
            }
            strehlData_Gauss = QSharedPointer<QCPGraphDataContainer>(new QCPGraphDataContainer);
            if (measurement_type == "DIMM"){
                std::cout << "init dimm" << std::endl;
                strehlData_DIMM = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);;
                horizontalVarData = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);;
                verticalVarData = QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);;
            }
        }
    }
    QSharedPointer<QCPGraphDataContainer> fried = nullptr;
    QSharedPointer<QCPGraphDataContainer> seeing = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_x = nullptr;
    QSharedPointer<QCPGraphDataContainer> centroid_y = nullptr;
    QSharedPointer<QCPBarsDataContainer> saturation = nullptr;
    QSharedPointer<QCPBarsDataContainer> strehlData_DIMM = nullptr;
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
         cm_residue(plot(layoutPosition(2,2,1,1), "residueColormap", labelContainer("x", "y", "residue")))
     {}
     scatterplot seeing;
     scatterplot fried;
     scatterplot centroid;
     histogram saturation;
     //histogram strehl;
     colorMap cm_image;
     colorMap cm_fit;
     colorMap cm_residue;
};


class SeeingGui : public QMainWindow
{
    Q_OBJECT
public:
    SeeingGui(QWidget *parent = nullptr);
    ~SeeingGui();

    std::atomic<bool> *isMeasuringSeeing = nullptr; ///< When set to true, cameragui starts uploading images to m_imageContainer.
    std::shared_ptr<datacontainers::ImageContainer> m_imageContainer = nullptr; ///< Image container for one DIMM sample. After each fried parameter is calculated, the container is emptied and filled again by cameragui.
    datacontainers::configurationSettings_seeingGui m_configurationSettings;

    void startMeasurement(); ///< Initiates a seeing measurement with the measurement settings in the gui.
    void stopMeasurement(); ///< Terminates the currently running seeing measurement. Handles the safe termination of the DIMM_thread, and stores the results.
    //void load_config(datacontainers::configurationSettings_seeingGui config_settings);

    void configureSettings();
private:
    Ui::SeeingGui *ui;

    seeingPlots plots;
    std::unique_ptr<plotDisplay> plotsWindow = nullptr;


    seeingData m_seeingData;// = nullptr;
    datacontainers::GaussSample m_GaussSample;
    datacontainers::DIMMsample m_DIMMsample; ///< Contains the spotseparations of a DIMM sample, as well as functions for calculating the average and variance of the sample.

    std::unique_ptr<std::thread> measurement_thread = nullptr;
    std::atomic<bool> isProcessing; ///< Flags that the real-time processing loop is active.


    time_t startOfMeasurement; ///< Timestamp at the start of the measurement
    //time_t endOfMeasurement; ///< Timestamp at the end of the measurement

    QString currentSubFolderName; ///< Holds the file path to the current subfolder in basedirectory. Each subfolder corresponds to a sample size, and hosts the images of the sample.
    QFile parameterFile;
    QTextStream writeParametersToFile;
    QFile resultsFile;
    QTextStream writeResults;

    void DIMM(); ///< Handles the real-time processing of images in m_imageContainer. DIMM() calculates the spotseparation of each image in m_imagecontainer and subsequently the fried parameter and seeing value when the sample size is reached. It also handles the storage of data according to the setup parameters, and continuously updates the gui display. DIMM() is run in a separate thread and terminates when stopMeasurement() is called.
    void Gaussian();
    void setup_plots_window();
    void setMeasurementSettings(); ///< Reads the measurement settings chosen in the gui.

    double getFriedParameter(); ///< Calculates the Fried parameter of m_DIMMsample.
    double getFriedParameter_x(); ///< Calculates the Fried parameter of m_DIMMsample.
    double getFriedParameter_y(); ///< Calculates the Fried parameter of m_DIMMsample.
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
    void debugDIMM(const cv::Mat& image);
    void setWindowRadius();
    void updateScatterPlots();
signals:
    void newSeeingValues(); ///< Signals that plots and display values in the gui are to be updated.
    void newMessage(QString message, bool error = false); ///< Emits a signal connected to displayMessage. Is needed within the DIMM_thread, as only the main thread can interact with the GUI.
    void debugGaussianSeeing(const cv::Mat &image);
};

#endif // SEEINGGUI_H

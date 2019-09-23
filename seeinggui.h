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
#include "imageprocessing.h"
#include "datacontainers.h"


namespace Ui{
class   SeeingGui;
}

/********************************************//**
 * SeeingGui is an interface for configuring and performing a seeing measurement.
 * The class is implemented with the DIMM measurement method, and assumes that the two spots from the DIMM measurement will locate in their respective left-right halves of the image.
 ***********************************************/

class SeeingGui : public QMainWindow
{
    Q_OBJECT

public:
    SeeingGui(QWidget *parent = nullptr);
    ~SeeingGui();

    std::atomic<bool> *isMeasuringSeeing = nullptr; ///< When set to true, cameragui starts uploading images to m_imageContainer.
    std::shared_ptr<datacontainers::ImageContainer> m_imageContainer = nullptr; ///< Image container for one DIMM sample. After each fried parameter is calculated, the container is emptied and filled again by cameragui.
    datacontainers::seeingParameters m_seeingParams; ///< Struct containing the settings of the seeing setup, such as the telescope optics and wavelength of the source.

    void startMeasurement(); ///< Initiates a seeing measurement with the measurement settings in the gui.
    void stopMeasurement(); ///< Terminates the currently running seeing measurement. Handles the safe termination of the DIMM_thread, and stores the results.

private:
    Ui::SeeingGui *ui;
    std::unique_ptr<std::thread> DIMM_thread = nullptr; ///< Thread handling the real-time image processing through the DIMM() function.

    std::atomic<bool> isProcessing; ///< Flags that the real-time processing loop is active.

    bool storeImages; ///< flags if the data from the DIMM measurement should be stored or thrown away.
    unsigned int sampleSize; ///< sets the number of images used as sample size for each data point.

    datacontainers::DIMMsample m_DIMMsample; ///< Contains the spotseparations of a DIMM sample, as well as functions for calculating the average and variance of the sample.
    datacontainers::seeingValues m_seeingValues; ///< Container for fried and seeing values implemented with QCPGraphData Qvectors for ease of plotting and max, min, mean functions.

    double getFriedParameter(); ///< Calculates the Fried parameter of m_DIMMsample.

    double getSeeingFromFried(double FriedParameter, double wavelength); ///< Converts a Fried parameter to a seeing value.

    int displayPrecision = 3; ///< Precision used for Qstring representation of the display values
    char displayFormat = 'f'; ///< Format used for Qstring representation of the display values
    time_t startOfMeasurement; ///< Timestamp at the start of the measurement
    time_t endOfMeasurement; ///< Timestamp at the end of the measurement
    QString measurementType; ///< Specifies measurement method (e.g. DIMM, Gaussian fit).

    QString directoryPath; ///< path to directory where images and results will be saved
    QDir baseDirectory; ///< directory structure of directoryPath.
    QString currentSubFolderName; ///< Holds the file path to the current subfolder in basedirectory. Each subfolder corresponds to a sample size, and hosts the images of the sample.
    QFile parameterFile;
    QTextStream writeParametersToFile;
    QFile resultsFile;
    QTextStream writeResults;

    void DIMM(); ///< Handles the real-time processing of images in m_imageContainer. DIMM() calculates the spotseparation of each image in m_imagecontainer and subsequently the fried parameter and seeing value when the sample size is reached. It also handles the storage of data according to the setup parameters, and continuously updates the gui display. DIMM() is run in a separate thread and terminates when stopMeasurement() is called.
    void plotFriedParameter(); ///< Updates the fried parameter plot in the gui.
    void plotSeeing(); ///< Updates the seeing plot in the gui.
    void initPlots(); ///< Initializes the seeing and fried parameter plots in the gui.
    void createParameterFile(); ///< Creates text file where the current seeing measurement parameters are written.
    void writeResultsToFile(); ///< Writes the results of the measurement to a text file.
    void setMeasurementSettings(); ///< Reads the measurement settings chosen in the gui.
    QString timestampToFolderName(time_t timestamp); ///< Formats a more readable folder name from the current unix timestamp.
    void setSecPerDataPoint(); ///< Displays the length in seconds of each measurement interval. I.e. the sample size times the exposure time of the camera.
private slots:
    void replotSeeing(); ///< Updates the plots of seeing values and fried parameters in the gui.
    void updateSeeingPanel(); ///< Updates the display of the maximum, minimum, mean and current fried parameter and seeing value in the gui.
    void displayMessage(QString message, bool error = false); ///< Displays a message in the Gui. If error is true, an error message is printed to stdout through qDebug().

signals:
    void newSeeingValues(); ///< Signals that plots and display values in the gui are to be updated.
    void newMessage(QString message, bool error = false); ///< Emits a signal connected to displayMessage. Is needed within the DIMM_thread, as only the main thread can interact with the GUI.
};

#endif // SEEINGGUI_H


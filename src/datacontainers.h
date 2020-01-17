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


/********************************************//**
 *  Container for thread-safe centroid sharing between cameragui and hexapodgui
 ***********************************************/
struct CentroidStabilization
{
   cv::Point homeCentroid;
   cv::Point currentCentroid;
   double homeX;
   double homeY;
   double meanCentroidX;
   double meanCentroidY;
   std::vector<cv::Point> centroidHistory;
   std::mutex lockCentroidHistory;
   void addCentroid(cv::Point centroid){
       std::lock_guard<std::mutex> guard(lockCentroidHistory);
       centroidHistory.push_back(centroid);
   }
   void emptyHistory(){
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

    std::vector<int> xSeparation; //pixel separation is always an int.
    std::vector<int> ySeparation; //pixel separation is always an int.
    cv::Point avgCentroid1 = cv::Point(0,0);
    cv::Point avgCentroid2 = cv::Point(0,0);
    std::vector<double> strehl1;
    std::vector<double> strehl2;

    inline double average_x(){
        double sum = 0.0;
        for ( size_t i = 0; i < xSeparation.size(); i++){
            sum+= xSeparation.at(i);
        }
        return (sum/xSeparation.size());
    }
    inline double average_y(){
        double sum = 0.0;
        for ( size_t i = 0; i < ySeparation.size(); i++){
            sum+= ySeparation.at(i);
        }
        return (sum/ySeparation.size());
    }
    inline double variance_x(){
        double sum = 0.0;
        double avg = average_x();
        for (  size_t j =0; j < xSeparation.size(); j++){
            sum+= std::pow((xSeparation[j] - avg),2);
        }
        return sum/(xSeparation.size() - 1); //divide by n-1 since this is an estimator of the real variance.
    }
    inline double variance_y(){
        double sum = 0.0;
        double avg = average_y();
        for (  size_t j =0; j < ySeparation.size(); j++)        {
            sum += std::pow((ySeparation[j] - avg),2);
        }
        return sum/(ySeparation.size() - 1); //divide by n-1 since this is an estimator of the real variance.
    }
};

struct gaussianFitParams{
    double intensitymax;
    double var_x;
    double var_y;
    double center_x;
    double center_y;
    double sigma_cov;
    bool valid;
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

}
#endif // DATACONTAINERS_H

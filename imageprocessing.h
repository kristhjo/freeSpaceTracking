#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H
#include <QImage>
#include <iostream>
#include <thread>
#include <opencv2/opencv.hpp>
#include <vector>
#include "datacontainers.h"
/********************************************//**
 *  Contains simple image manipulation algorithms for the centroid and differential image motion calculations.
 ***********************************************/
namespace imageprocessing{
void cropWindow(const cv::Mat &img, cv::Mat &croppedImg, int windowRadius); ///< Crops the input image around its brightest spot with a circle of radius windowRadius.
void cropThreshold(const cv::Mat &img, cv::Mat &croppedImg, int trackingThresh); ///< Crops image pixels with brightness below trackingThresh.
cv::Point findCentroid(const cv::Mat &img); ///< Returns the coordinates of the brightest pixel in the image.
cv::Point getSpotSeparation(const cv::Mat &img, int windowRadius); ///< Returns the coordinate distance (x,y) between the brightest spot in each half (split left-right) of the image.
datacontainers::gaussianFitParams getGaussianFitParams(const cv::Mat &img);
void drawGaussian(cv::Mat &gaussImg, datacontainers::gaussianFitParams fitParams);
double Strehl_ratio(cv::Mat &img);
}
#endif // IMAGEPROCESSING_H

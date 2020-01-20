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
datacontainers::gaussianFitParams getGaussianFitParams(const cv::Mat img);
double getStrehlRatio(const cv::Mat &img, double pixel_size, double wavelength, double aperture_diameter);
void getDIMM_strehlRatio(const cv::Mat &img, double& strehl1, double& strehl2, double pixel_size_radians, double wavelength, double aperture_diameter, int cropRadius);
void computeHistogram(const cv::Mat &img, cv::Mat &histogram);
void getDIMMcentroids(const cv::Mat &img, cv::Point& centroid1, cv::Point& centroid2);
void getSpotSeparation(const cv::Mat &img, int windowRadius, double& x_separation, double& y_separation, cv::Point& centroid1, cv::Point& centroid2); ///< Returns the coordinate distance (x,y) between the brightest spot in each half (split left-right) of the image.

}
#endif // IMAGEPROCESSING_H

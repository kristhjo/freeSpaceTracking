//#include <math>
#include "imageprocessing.h"
#include "datacontainers.h"

namespace imageprocessing {


void cropWindow(const cv::Mat &img, cv::Mat &croppedImg, int windowRadius = 5){
    cv::Mat temp, mask;
    cv::GaussianBlur(img, temp, cv::Size(3,3),0,0); //applies a gaussian blur to the image. Noise peaks are smeared out, thus finding the peak more reliably.
    double min, max;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(temp, &min, &max, &minLoc, &maxLoc);
    mask = cv::Mat (img.rows, img.cols, CV_8UC1, cv::Scalar(0,0,0));
    cv::circle(mask, maxLoc, windowRadius, cv::Scalar(128,0,0), -1); //Creates a circle around the maxLoc position in mask, with radius windowRadius.
    img.copyTo(croppedImg, mask); //Initiates croppedImg with  matrix resulting from img filtered with mask.
}


void cropThreshold(const cv::Mat &img, cv::Mat &croppedImg, int trackingThresh){
    cv::Mat mask;
    cv::threshold(img,mask,trackingThresh,1,cv::THRESH_BINARY); //Initiates mask with 1 where img is above trackingThresh and 0 elsewhere.
    img.copyTo(croppedImg, mask); //Copies img to croppedImg, where mask is applied so that croppedImg is 0 where mask is 0.
}

cv::Point findCentroid(const cv::Mat &img){
    cv::Moments m = moments(img,true);
    cv::Point centroid(m.m10/m.m00, m.m01/m.m00); //x and y centroid coordinates calculated from the first order moments / total intensity
    return centroid;
}

cv::Point getSpotSeparation(const cv::Mat &img, int windowRadius = 5){
    cv::Mat img1, img2, croppedImg1, croppedImg2;
    img1 = img(cv::Rect(0, 0, img.cols/2, img.rows)); //splits the original image in left and right halves. This assumes the spots are distributed this way, and not for example top-bottom.
    img2 = img(cv::Rect(img.cols/2, 0, img.cols/2, img.rows));
    cropWindow(img1, croppedImg1, windowRadius = windowRadius);
    cropWindow(img2, croppedImg2, windowRadius = windowRadius);
    cv::Point c1, c2;
    c1 = findCentroid(croppedImg1);
    c2 = findCentroid(croppedImg2);
    double min, max1, max2;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(img, &min, &max1, &minLoc, &maxLoc);
    cv::minMaxLoc(img, &min, &max2, &minLoc, &maxLoc);
    if (max1 <10 || max2 <10){
        return cv::Point(10000,10000);
    }
    cv::Point dist;
    dist.x = c2.x + img.cols/2 - c1.x;
    dist.y = c2.y - c1.y;
    return dist;
}

datacontainers::gaussianFitParams getGaussianFitParams(const cv::Mat &img){

    cv::Moments m = moments(img,true);
    datacontainers::gaussianFitParams params;
    cv::Point center(m.m10/m.m00, m.m01/m.m00);
    double min, max1;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(img, &min, &max1, &minLoc, &maxLoc);
    params.intensitymax = max1;
    params.center_x = center.x;
    params.center_y = center.y;
    double varx = 0;
    double vary = 0;
    double sumx=0;
    double sumy=0;
    double meanx = 0;
    ushort normFactor = img.at<ushort>(cv::Point(center.x, center.y));
    for (int i = 0; i < img.rows; i++){
        meanx+= i*img.at<ushort>(cv::Point(i, center.y));
        sumx+=img.at<ushort>(cv::Point(i, center.y));
        varx+= pow( center.x - i, 2)*img.at<ushort>(cv::Point(i, center.y));
    }
    std::cout << center.x << " center " << meanx/sumx << std::endl;
    for (int j = 0; j < img.cols; j++){
        sumy+=img.at<ushort>(cv::Point(center.x, j));
        vary+= pow( center.y - j, 2)*img.at<ushort>(cv::Point(center.x, j));
    }
    //std::cout << img << std::endl;
    std::cout << img.at<ushort>(cv::Point(center.x, center.y)) << " " <<  img.at<ushort>(cv::Point(0, center.y))  << " " << img.at<ushort>(cv::Point(center.x, 0))  << std::endl;

    std::cout << m.m20 << " " <<  m.m02  << " " << m.m11 << " moments " << normFactor << " " << max1 << " " << maxLoc << std::endl;
    params.var_x = varx/sumx;//((img.rows-1));//m.m20/m.m00;//pow(m.m20,0.5)*2.355; //sigma to FWHM conversion
    params.var_y = vary/sumy;//pow(m.m02, 0.5)*2.355; //sigma to FWHM conversion
    std::cout << sumx << " " <<  varx  << " " << sumy << " " << vary << std::endl;
    params.sigma_cov = m.m11;
    params.numSaturatedPixels = cv::countNonZero(img == 255);

    return params;
}

}

#include "imageprocessing.h"

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
    cv::Point dist;
    dist.x = c2.x + img.cols/2 - c1.x;
    dist.y = c2.y - c1.y;
    return dist;
}

}

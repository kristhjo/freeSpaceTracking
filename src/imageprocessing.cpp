//#include <math>
#include "imageprocessing.h"
#include "datacontainers.h"
#include <math.h>
namespace imageprocessing {


void cropWindow(const cv::Mat &img, cv::Mat &croppedImg, int windowRadius){
    ///defined for grayscale only
    cv::Mat smoothedImg(img.rows, img.cols,img.type(),cv::Scalar(0));
    croppedImg = cv::Mat(img.rows, img.cols, img.type(), cv::Scalar(0));
    cv::Mat mask(img.rows, img.cols, CV_8UC1, cv::Scalar(0));

    cv::GaussianBlur(img, smoothedImg, cv::Size(3,3),0,0); //applies a gaussian blur to the image. Noise peaks are smeared out, thus finding the peak more reliably.

    double min, max;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(smoothedImg, &min, &max, &minLoc, &maxLoc);
    cv::circle(mask, maxLoc, windowRadius, cv::Scalar(128), -1); //Creates a circle around the maxLoc position in mask, with radius windowRadius.
    img.copyTo(croppedImg, mask); //Initiates croppedImg with  matrix resulting from img filtered with mask.
}


void cropThreshold(const cv::Mat &img, cv::Mat &croppedImg, int trackingThresh){
    ///defined for 8UC1 only
    cv::Mat mask;
    cv::threshold(img,mask,trackingThresh,1,cv::THRESH_BINARY); //Initiates mask with 1 where img is above trackingThresh and 0 elsewhere.
    img.copyTo(croppedImg, mask); //Copies img to croppedImg, where mask is applied so that croppedImg is 0 where mask is 0.
}

cv::Point findCentroid(const cv::Mat &img){
    cv::Moments m = moments(img,true);
    cv::Point centroid(m.m10/m.m00, m.m01/m.m00); //x and y centroid coordinates calculated from the first order moments / total intensity
    return centroid;
}

void getDIMMcentroids(const cv::Mat &img, cv::Point& centroid1, cv::Point& centroid2){
    cv::Mat img1, img2, croppedImg1, croppedImg2;
    img1 = img(cv::Rect(0, 0, img.cols/2, img.rows)); //splits the original image in left and right halves. This assumes the spots are distributed this way, and not for example top-bottom.
    img2 = img(cv::Rect(img.cols/2, 0, img.cols/2, img.rows));
    cropWindow(img1, croppedImg1, 10);
    cropWindow(img2, croppedImg2, 10);
    centroid1 = findCentroid(croppedImg1);
    centroid2 = findCentroid(croppedImg2);
}

void getSpotSeparation(const cv::Mat &img, int windowRadius, double& x_separation, double& y_separation, cv::Point& centroid1, cv::Point& centroid2){
    cv::Mat img1, img2, croppedImg1, croppedImg2;
    img1 = img(cv::Rect(0, 0, img.cols/2, img.rows)); //splits the original image in left and right halves. This assumes the spots are distributed this way, and not for example top-bottom.
    img2 = img(cv::Rect(img.cols/2, 0, img.cols/2, img.rows));
    cropWindow(img1, croppedImg1, windowRadius);
    cropWindow(img2, croppedImg2, windowRadius);
    centroid1 = findCentroid(croppedImg1);
    centroid2 = findCentroid(croppedImg2);
    x_separation = centroid2.x + img.cols -centroid1.x;
    y_separation = centroid2.y - centroid1.y;
}

void getDIMM_strehlRatio(const cv::Mat &img, double& strehl1, double& strehl2, double pixel_size_radians, double wavelength, double aperture_diameter, int cropRadius){
    //pixel size in radians
    cv::Mat img1, img2, croppedImg1, croppedImg2;
    img1 = img(cv::Rect(0, 0, img.cols/2, img.rows)); //splits the original image in left and right halves. This assumes the spots are distributed this way, and not for example top-bottom.
    img2 = img(cv::Rect(img.cols/2, 0, img.cols/2, img.rows));
    cropWindow(img1, croppedImg1, cropRadius);
    cropWindow(img2, croppedImg2, cropRadius);

    double min1, max1, min2, max2;
    cv::Point minLoc1, maxLoc1, minLoc2, maxLoc2;
    cv::minMaxLoc(croppedImg1, &min1, &max1, &minLoc1, &maxLoc1);
    cv::minMaxLoc(croppedImg2, &min2, &max2, &minLoc2, &maxLoc2);
    double total_intensity1 = cv::sum(croppedImg1)[0];
    double total_intensity2 = cv::sum(croppedImg2)[0];
    std::cout << " from strehl:\n";
    std::cout << "max1: " << max1 << " total_intensity1: " << total_intensity1 << std::endl;
    std::cout << "max2: " << max1 << " total_intensity2: " << total_intensity1 << std::endl;
    strehl1 =  (max1/total_intensity1)*(4/M_PI)*pow(wavelength/(pixel_size_radians*aperture_diameter), 2);
    strehl2 =  (max2/total_intensity2)*(4/M_PI)*pow(wavelength/(pixel_size_radians*aperture_diameter), 2);
    std::cout << "strehl1: " << strehl1 << " strehl2: " << strehl2 << std::endl;
}

datacontainers::gaussianFitParams getGaussianFitParams(const cv::Mat img){
    cv::Moments m = moments(img);
    datacontainers::gaussianFitParams params;
    cv::Point center(static_cast<int>(m.m10/m.m00), static_cast<int>(m.m01/m.m00));
    if ( (abs(center.y) > img.rows )|| (abs(center.x) > img.cols )){
        std::cout << "invalid image" << std::endl;
        params.valid = false;
        return params;
    }
    else if( std::isnan(m.m10/m.m00) || std::isnan(m.m01/m.m00)){
        std::cout << "invalid image" << std::endl;
        params.valid = false;
        return params;
    }
    else {
        params.valid = true;
    }
    double min, max1;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(img, &min, &max1, &minLoc, &maxLoc);
    params.center_x = m.m10/m.m00; //center.x;
    params.center_y = m.m01/m.m00; //center.y;
    params.intensitymax = img.at<ushort>(cv::Point(center.x, center.y));//max1;

    double varx=0.0, vary=0.0, sumx = 0.0, sumy = 0.0;

    //ushort normFactor = img.at<ushort>(cv::Point(center.x, center.y));
    for (int i = 0; i < img.rows; i++){
        sumx+=img.at<ushort>(cv::Point(i, center.y));
        varx+= pow( center.x - i, 2)*img.at<ushort>(cv::Point(i, center.y));
    }
    for (int j = 0; j < img.cols; j++){
        sumy+=img.at<ushort>(cv::Point(center.x, j));
        vary+= pow( center.y - j, 2)*img.at<ushort>(cv::Point(center.x, j));
    }


    params.var_x = varx/sumx;
    params.var_y = vary/sumy;

    std::cout << vary << " " << sumy << std::endl;
    std::cout << varx << " " << sumx << std::endl;
    std::cout << m.m10 << " " << m.m01 << " " << m.m00 << std::endl;
    //params.sigma_cov = m.m11;
    std::cout << " variations " << params.var_x << " " << params.var_y << " " << params.sigma_cov << std::endl;
    return params;
}

double getStrehlRatio(const cv::Mat &img, double pixel_size, double wavelength, double aperture_diameter){
    //pixel size in radians
    cv::Mat croppedImg;
    cv::Mat mask;
    cv::Mat tempImg;
    img.copyTo(tempImg);
    tempImg.convertTo(tempImg, CV_8UC1);
    //cv::convertScaleAbs(img,scaledImg);
    cv::threshold(tempImg,mask,10,1,cv::THRESH_BINARY); //Initiates mask with 1 where img is above trackingThresh and 0 elsewhere.
    img.copyTo(croppedImg, mask);

    double min, max;
    cv::Point minLoc, maxLoc;
    cv::minMaxLoc(croppedImg, &min, &max, &minLoc, &maxLoc);
    double total_intensity = cv::sum(croppedImg)[0];
    return (max/total_intensity)*(4/M_PI)*pow(wavelength/(pixel_size*aperture_diameter), 2);
}


void computeHistogram(const cv::Mat &img, cv::Mat &histogram) {
    int channels =  0;
    int histSize =  256;
    float range[] = { 1, 256 };
    const float* histRange = { range };
    bool unifrom_bins = true;
    bool accumulate = false;
    cv::calcHist(&img, 1, &channels, cv::Mat(), histogram, 1, &histSize, &histRange, unifrom_bins, accumulate);
    //cv::normalize(histogram, histogram, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());//img.rows*img.cols
}

}

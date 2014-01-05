#include "ImageComponentExtractor.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

ImageComponentExtractor::ImageComponentExtractor(Mat image) {
    this->image = image;
}

void ImageComponentExtractor::demo_contours(int mode, int threshold_begin, int threshold_end, int thresh_mode) {
    //Prepare the image for findContours
    Mat grayImg = this->image.clone();
    cvtColor(grayImg, grayImg, CV_BGR2GRAY);
    if (thresh_mode != 5) threshold(grayImg, grayImg, threshold_begin, threshold_end, thresh_mode);

    //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
    vector<vector<Point>> contours;
    Mat contourOutput = grayImg.clone();
    findContours( contourOutput, contours, CV_RETR_LIST, mode );

    //Draw the contours
    Mat contourImage(grayImg.size(), CV_8UC3, Scalar(0,0,0));
    Scalar colors[3];
    colors[0] = Scalar(255, 0, 0);
    colors[1] = Scalar(0, 255, 0);
    colors[2] = Scalar(0, 0, 255);
    for (size_t idx = 0; idx < contours.size(); idx++) {
        drawContours(contourImage, contours, idx, colors[idx % 3]);
    }

    imshow("Input Image", this->image);
    imshow("Contours", contourImage);
    waitKey(0);
}

void ImageComponentExtractor::demo_edges(double threshold, int apertureSize, bool L2gradient, int ratio, bool useBlur) {
    Mat src = this->image.clone();
    Mat dst;
    dst = Scalar::all(0);
    Mat detected_edges;
    cvtColor(src, detected_edges, CV_BGR2GRAY);
    if (useBlur) blur( detected_edges, detected_edges, Size(apertureSize,apertureSize) );
    Canny( detected_edges, detected_edges, threshold, threshold*ratio, apertureSize, L2gradient);
    src.copyTo( dst, detected_edges);
    imshow("Edges", dst);
    waitKey(0);
}

void ImageComponentExtractor::demo_contours_using_edges(
                                        int mode,
                                        int threshold_begin,
                                        int threshold_end,
                                        int thresh_mode,
                                        double threshold,
                                        int apertureSize,
                                        bool L2gradient,
                                        int ratio,
                                        bool useBlur) {
    //Prepare the image for findContours
    Mat grayImg = this->image.clone();
    Mat detected_edges;
    cvtColor(grayImg, detected_edges, CV_BGR2GRAY);
    if (thresh_mode != 5) cv::threshold(detected_edges, detected_edges, threshold_begin, threshold_end, thresh_mode);
    if (useBlur) blur( detected_edges, detected_edges, Size(apertureSize,apertureSize) );
    //Find the contours. Use the contourOutput Mat so the original image doesn't get overwritten
    vector<vector<Point>> contours;

    Canny( detected_edges, detected_edges, threshold, threshold*ratio, apertureSize, L2gradient);
    findContours( detected_edges, contours, CV_RETR_LIST, mode );

    //Draw the contours
    Mat contourImage(grayImg.size(), CV_8UC3, Scalar(0,0,0));
    Scalar colors[3];
    colors[0] = Scalar(255, 0, 0);
    colors[1] = Scalar(0, 255, 0);
    colors[2] = Scalar(0, 0, 255);
    for (size_t idx = 0; idx < contours.size(); idx++) {
        drawContours(contourImage, contours, idx, colors[idx % 3]);
    }

    imshow("Input Image", this->image);
    imshow("Contours", contourImage);
    waitKey(0);
}

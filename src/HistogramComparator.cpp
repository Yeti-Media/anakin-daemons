#include "HistogramComparator.hpp"
#include <DataInput.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

HistogramComparator::HistogramComparator(Img* scene) {
    this->scene = scene;
}

Mat HistogramComparator::train(DataInput* input) {

    Img* current;
    Mat b_hist, g_hist, r_hist;
    /// Establish the number of bins
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 256 } ;
    const float* histRange = { range };
    float channels[] = { 0, 2 } ;
    float* histRange_result[] = { channels, range};

    bool uniform = true; bool accumulate = true;

    int imgCount = 0;

    Mat result = Mat::zeros(histSize, 3, CV_32SC1);

    while (input->nextInput(&current)) {

        Mat src = current->getImage();
        /// Separate the image in 3 places ( B, G and R )
        vector<Mat> bgr_planes;
        split( src, bgr_planes );

        /// Compute the histograms:
        calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
        calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
        normalize(b_hist, b_hist, 0, histSize, NORM_MINMAX, -1, Mat() );
        normalize(g_hist, g_hist, 0, histSize, NORM_MINMAX, -1, Mat() );
        normalize(r_hist, r_hist, 0, histSize, NORM_MINMAX, -1, Mat() );

        // Draw the histograms for B, G and R
        int hist_w = 512; int hist_h = 400;
        int bin_w = cvRound( (double) hist_w/histSize );

        for (int i = 0; i < histSize-1; i++) {
            float b_bin_val = b_hist.at<float>(i);
            float g_bin_val = g_hist.at<float>(i);
            float r_bin_val = r_hist.at<float>(i);
            result.at<float>(i, 0) += b_bin_val;
            result.at<float>(i, 1) += g_bin_val;
            result.at<float>(i, 2) += r_bin_val;
        }

        Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );
        Mat currentHist(hist_h, hist_w, CV_8UC3, Scalar( 0,0,0));
        /// Draw for each channel
        for( int i = 1; i < histSize; i++ ) {
            float b_bin_val = result.at<float>(i, 0);
            //b_bin_val = imgCount == 0? b_bin_val : b_bin_val / imgCount;

            float g_bin_val = result.at<float>(i, 1);
            //g_bin_val = imgCount == 0? g_bin_val : g_bin_val / imgCount;

            float r_bin_val = result.at<float>(i, 2);
            //r_bin_val = imgCount == 0? r_bin_val : r_bin_val / imgCount;

            float b_bin_val_prev = result.at<float>(i-1, 0);
            //b_bin_val_prev = imgCount == 0? b_bin_val_prev : b_bin_val_prev / imgCount;

            float g_bin_val_prev = result.at<float>(i-1, 1);;
            //g_bin_val_prev = imgCount == 0? g_bin_val_prev : g_bin_val_prev / imgCount;

            float r_bin_val_prev = result.at<float>(i-1, 2);
            //r_bin_val_prev = imgCount == 0? r_bin_val_prev : r_bin_val_prev / imgCount;

            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_bin_val_prev) ) ,
                           Point( bin_w*(i), hist_h - cvRound(b_bin_val) ),
                           Scalar( 255, 0, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_bin_val_prev) ) ,
                           Point( bin_w*(i), hist_h - cvRound(g_bin_val) ),
                           Scalar( 0, 255, 0), 2, 8, 0  );
            line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_bin_val_prev) ) ,
                           Point( bin_w*(i), hist_h - cvRound(r_bin_val) ),
                           Scalar( 0, 0, 255), 2, 8, 0  );


            line( currentHist, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
                     Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                     Scalar( 255, 0, 0), 2, 8, 0  );
            line( currentHist, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
                     Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                     Scalar( 0, 255, 0), 2, 8, 0  );
            line( currentHist, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
                     Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                     Scalar( 0, 0, 255), 2, 8, 0  );
        }

        /// Display
        namedWindow("average hist", CV_WINDOW_AUTOSIZE );
        imshow("average hist", histImage );

        namedWindow("current hist", CV_WINDOW_AUTOSIZE );
        imshow("current hist", currentHist );

        namedWindow("current img", CV_WINDOW_AUTOSIZE );
        imshow("current img", src );

        waitKey(0);

        imgCount++;

    }



    return result;
}

double HistogramComparator::compareUsingColor(Img* pattern, int method) {
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, histSize-1 };
    const float* ranges[] = { range, range, range};

    bool uniform = true; bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0, 1, 2};

    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg = this->scene->getImage();
    Mat patternImg = pattern->getImage();

    /// Calculate the histograms for the BGR images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 1, &histSize, ranges, uniform, false );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 1, &histSize, ranges, uniform, false );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == 0) result *= 100;
    else if (method == 2) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

double HistogramComparator::compareUsingGray(Img* pattern, int method) {
    int histSize = 256;

    /// Set the ranges ( for greyscale) )
    float range[] = { 0, histSize-1 };
    const float* ranges[] = { range};

    bool uniform = true; bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0};

    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg = this->scene->getGrayImg();
    Mat patternImg = pattern->getGrayImg();

    /// Calculate the histograms for the grayscale images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 1, &histSize, ranges, uniform, false );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 1, &histSize, ranges, uniform, false );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == 0) result *= 100;
    else if (method == 2) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

double HistogramComparator::compareUsingHSV(Img* pattern, int method) {
    int h_bins = 50; int s_bins = 32;
    int histSize[] = { h_bins, s_bins };

    float h_ranges[] = { 0, 256 };
    float s_ranges[] = { 0, 180 };

    bool uniform = true; bool accumulate = false;

    const float* ranges[] = { h_ranges, s_ranges };

    int channels[] = { 0, 1 };


    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg;
    Mat patternImg;
    cvtColor( this->scene->getImage(), sceneImg, CV_BGR2HSV );
    cvtColor( pattern->getImage(), patternImg, CV_BGR2HSV );

    /// Calculate the histograms for the grayscale images
    calcHist( &sceneImg, 1, channels, Mat(), hist_scene, 2, histSize, ranges, uniform, false );
    normalize( hist_scene, hist_scene, 0, 1, NORM_MINMAX, -1, Mat() );

    calcHist( &patternImg, 1, channels, Mat(), hist_pattern, 2, histSize, ranges, uniform, false );
    normalize( hist_pattern, hist_pattern, 0, 1, NORM_MINMAX, -1, Mat() );

    double result = compareHist( hist_scene, hist_pattern, method );

    if (method == 0) result *= 100;
    else if (method == 2) {
        double maxVal = compareHist( hist_scene, hist_scene, method );
        result = (result * 100) / maxVal;
    }

    return result;
}

HistogramComparator::~HistogramComparator() {
    delete this->scene;
}

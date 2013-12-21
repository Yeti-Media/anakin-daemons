#include "HistogramComparator.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

HistogramComparator::HistogramComparator(Img* scene) {
    this->scene = scene;
}

double HistogramComparator::compareUsingColor(Img* pattern, int method) {
    int histSize = 256;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, histSize };
    const float* ranges[] = { range, range, range};

    bool uniform = true; bool accumulate = false;

    // Use the o-th and 1-st channels
    int channels[] = { 0, 1, 2};

    /// Histograms
    MatND hist_scene;
    MatND hist_pattern;
    Mat sceneImg = this->scene->getImage();
    Mat patternImg = pattern->getImage();

    /// Calculate the histograms for the HSV images
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
    return 0;
}

double HistogramComparator::compareUsingHSV(Img* pattern, int method) {
    return 0;
}

HistogramComparator::~HistogramComparator() {
    delete this->scene;
}

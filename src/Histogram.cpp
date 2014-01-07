#include "Histogram.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

Histogram::Histogram(Mat hist, vector<int>* bins, int channels, string label="" ) {
    this->hist = hist;
    this->bins = bins;
    this->channels = channels;
    this->label = label;
}


Mat Histogram::getHist() {
    return this->hist;
}

vector<int>* Histogram::getBins() {
    return this->bins;
}

int Histogram::getChannels() {
    return this->channels;
}

string Histogram::getLabel() {
    return this->label;
}

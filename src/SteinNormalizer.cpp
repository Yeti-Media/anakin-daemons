#include "SteinNormalizer.hpp"
#include <pthread.h>

using namespace Anakin;
using namespace std;
using namespace cv;

void SteinNormalizer::printSegments(vector<vector<int>*>* segments) {
    cout << "[\n";
    for (int s = 0; s < segments->size(); s++) {
        cout << "[";
        vector<int>* currentSegment = segments->at(s);
        for (int v = 0; v < currentSegment->size(); v++) {
            cout << currentSegment->at(v);
            if (v+1 < currentSegment->size()) {
                cout << ", ";
            }
        }
        cout << "]\n";
    }
    cout << "]\n";
}

void SteinNormalizer::printMeanPerValue(vector<vector<int>*>* segments) {
    for (int v = 0; v < 256; v++) {
        cout << "mean for value (" << v << ") is (" << getMean(getSegmentValues(v)) << ")\n";
    }
}

SteinNormalizer::SteinNormalizer(cv::Mat img, int segmentSize) {
    this->sourceImg = img;
    bool lastOneIsBigger = false;
    if (256%segmentSize == 0) {
        this->segments = 256 / segmentSize;
    } else {
        this->segments = floor(256.0 / (float) segmentSize);
        lastOneIsBigger = true;
    }
    if (segmentSize%2 == 0 || segmentSize > 256 || segmentSize < 1) {
        cout << "segments must be an odd value between 1 and 256\n";
        exit(-1);
    }
    this->segmentSize = segmentSize;
    this->segmentsValues = new vector<vector<int>*>(this->segments);
    int currentValue = 0;
    for (int s = 0; s < this->segments; s++) {
        int sSize = s+1 == this->segments && lastOneIsBigger? segmentSize+1 : segmentSize;
        vector<int>* seg = new vector<int>(sSize);
        for (int v = 0; v < sSize; v++) {
            seg->at(v) = currentValue;
            currentValue++;
        }
        this->segmentsValues->at(s) = seg;
    }
    //printSegments(this->segmentsValues);
    //printMeanPerValue(this->segmentsValues);
}

SteinNormalizer::SteinNormalizer() {
    cout << "you are using the testing constructor\n" << endl;
}

Mat SteinNormalizer::normalize() {
    Mat normalizedImage = this->sourceImg.clone();
    for(int r=0;r<sourceImg.rows;r++) {
        for (int c=0;c<sourceImg.cols;c++) {
            int currentValue = sourceImg.at<uchar>(r,c);
            normalizedImage.at<uchar>(r,c) = getMean(getSegmentValues(currentValue));
        }
    }
    return normalizedImage;
}

vector<Point>* SteinNormalizer::getNeighbors(const Point pos) {
    vector<Point>* neighbors = new vector<Point>(0);
    return neighbors;
}

int SteinNormalizer::getMaxLabel(vector<Point>* neighbors) {
    return 0;
}

vector<int>* SteinNormalizer::getSegmentValues(const int value) {
    //cout << "getSegmentsValues(" << value << ")\n";
    int idx = value == 0? 0 : floor((float)value/(float)this->segmentSize);
    if (idx == this->segments) {
        idx--;
    }
    //cout << "segmentsValues->size() : " << this->segmentsValues->size() << " | idx : " << idx << "\n";
    return this->segmentsValues->at(idx);
}

int SteinNormalizer::getMean(vector<int>* values) {
    int idx = floor((float)values->size()/2.0);
    //cout << "values->size() : " << values->size() << " | idx : " << idx << "\n";
    return values->at(idx);
}

bool SteinNormalizer::areInSameSegment(const int v1, const int v2) {
    return false;
}

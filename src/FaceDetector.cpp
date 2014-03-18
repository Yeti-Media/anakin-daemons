#include "FaceDetector.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

FaceDetector::FaceDetector(std::string mainCascadeClassifierData) {
    this->mainClassifier = new CascadeClassifier();
    load(mainCascadeClassifierData, this->mainClassifier);
    this->forceIgnoreDetails = true;
    this->mainLabel = getFilename(mainCascadeClassifierData);
}

FaceDetector::FaceDetector(string mainCascadeClassifierData, vector<string>* detailsCascadeClassifierData) {
    this->mainClassifier = new CascadeClassifier();
    this->detailsClassifiers = new vector<CascadeClassifier*>(0);
    load(mainCascadeClassifierData, this->mainClassifier);
    this->mainLabel = getFilename(mainCascadeClassifierData);
    this->detailsLabels = new vector<string>(0);
    for (uint c = 0; c < detailsCascadeClassifierData->size(); c++) {
        CascadeClassifier* detailsClassifier = new CascadeClassifier();
        load(detailsCascadeClassifierData->at(c), detailsClassifier);
        this->detailsClassifiers->push_back(detailsClassifier);
        this->detailsLabels->push_back(getFilename(detailsCascadeClassifierData->at(c)));
    }
}

void FaceDetector::load(string file, CascadeClassifier* classifier) {
    if (!classifier->load(file)) {cout << "Couldn't load " << file.c_str() << "\n"; exit(-1);}
}

vector<FaceMatch*>* FaceDetector::detect(Mat image, vector<Rect>* mainDetections, vector<vector<Rect>*>* details, bool lookForDetails) {
    vector<FaceMatch*>* results = new vector<FaceMatch*>(0);
    detect(image, mainDetections);
    for (uint m = 0; m < mainDetections->size(); m++) {
        vector<pair<string, vector<Rect>*>*>* detailsResults = new vector<pair<string, vector<Rect>*>*>(0);
        vector<Rect>* detailsOffMain = new vector<Rect>(0);
        for (uint cc = 0; cc < this->detailsClassifiers->size() && !(this->forceIgnoreDetails || !lookForDetails); cc++) {
            Rect mainRect = mainDetections->at(m);
            Mat mainRoi = image(mainRect);
            Size mainSize(mainRect.width, mainRect.height);
            resize(mainRoi, mainRoi, mainSize);
            //namedWindow("face", CV_WINDOW_AUTOSIZE);
            //imshow("face", mainRoi);
            //waitKey();
            detect_default(mainRoi, this->detailsClassifiers->at(cc), detailsOffMain);
            details->push_back(detailsOffMain);
            string label = this->detailsLabels->at(cc);
            pair<string, vector<Rect>*>* detailsResult = new pair<string, vector<Rect>*>(label, detailsOffMain);
            detailsResults->push_back(detailsResult);
        }
        pair<string, Rect>* mainResult = new pair<string, Rect>(this->mainLabel, mainDetections->at(m));
        FaceMatch* match = new FaceMatch(mainResult, detailsResults);
        results->push_back(match);
    }
    return results;
}

void FaceDetector::detect(Mat image, vector<Rect>* mainDetections) {
    detect(image, this->mainClassifier, mainDetections);
}

void FaceDetector::detect(Mat image, CascadeClassifier* classifier, vector<Rect>* detections) {
    classifier->detectMultiScale(image, *detections, this->scaleFactor, this->minNeighbors, 0, this->minSize, this->maxSize);
}

void FaceDetector::detect_default(Mat image, CascadeClassifier* classifier, vector<Rect>* detections) {
    classifier->detectMultiScale(image, *detections, this->scaleFactor, this->minNeighbors);
}

void FaceDetector::showDetections(cv::Mat image, vector<FaceMatch*>* matches) {
    namedWindow("detections", CV_WINDOW_AUTOSIZE);
    int fontFace = FONT_HERSHEY_PLAIN;
    double fontScale = 1;
    int thickness = 1;
    for (uint m = 0; m < matches->size(); m++) {
        FaceMatch* match = matches->at(m);
        string mainLabel = match->getMain()->first;
        Rect rect = match->getMain()->second;
        Point p1(rect.x, rect.y);
        Point p2(p1.x+rect.width, p1.y+rect.height);

        //DRAW MAIN RECT AND LABEL
        rectangle(image, p1, p2, Scalar(0,0,255), 2, 8);
        putText(image, mainLabel, Point(p1.x, p1.y-5), fontFace, fontScale, Scalar(0,0,255), thickness, 8);

        vector<pair<string, vector<Rect>*>*>* details = match->getDetails();
        for (uint d = 0; d < details->size(); d++) {
            pair<string, vector<Rect>*>* detail = details->at(d);
            string detailLabel = detail->first;
            vector<Rect>* dRects = detail->second;
            for (uint dr = 0; dr < dRects->size(); dr++) {
                Rect dRect = dRects->at(dr);
                Point dp1(p1.x+dRect.x, p1.y+dRect.y);
                Point dp2(dp1.x+dRect.width, dp1.y+dRect.height);

                //DRAW DETAIL AND DETAIL LABEL
                rectangle(image, dp1, dp2, Scalar(0,255,0), 2, 8);
                putText(image, detailLabel, Point(dp1.x, dp1.y-5), fontFace, fontScale, Scalar(0,255,0), thickness, 8);
            }
        }
    }
    imshow("detections", image);
    waitKey();
}

void FaceDetector::setScaleFactor(float value) {
    this->scaleFactor = value;
}

void FaceDetector::setMinNeighbors(int value) {
    this->minNeighbors = value;
}

void FaceDetector::setMinSize(Size value) {
    this->minSize = value;
}

void FaceDetector::setMaxSize(Size value) {
    this->maxSize = value;
}

float FaceDetector::getScaleFactor() {
    return this->scaleFactor;
}

int FaceDetector::getMinNeighbors() {
    return this->minNeighbors;
}

Size FaceDetector::getMinSize() {
    return this->minSize;
}

Size FaceDetector::getMaxSize() {
    return this->maxSize;
}

string FaceDetector::getFilename (const string& str) {
    unsigned lastSeparator = str.find_last_of("/\\");
    string file = lastSeparator == string::npos ? str : str.substr(lastSeparator+1);
    unsigned lastDot = file.find_last_of(".");
    string name = lastDot == string::npos ? file : file.substr(0, lastDot);
    return name;
}

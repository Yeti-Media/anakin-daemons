#include "ImageInfo.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

ImageInfo::ImageInfo(string label, vector<KeyPoint> keypoints, Mat descriptors) {
    this->label = label;
    this->keypoints = keypoints;
    this->descriptors = descriptors;
}

ImageInfo::ImageInfo() {
    this->label = "DEFAULT";
    this->keypoints = vector<KeyPoint>(0);
    this->descriptors = Mat();
}

ImageInfo::~ImageInfo() {
    //dtor
}

void ImageInfo::setLabel(std::string l) {
    this->label = l;
}

string ImageInfo::getLabel() {
    return this->label;
}

vector<KeyPoint> ImageInfo::getKeypoints() {
    return this->keypoints;
}

Mat ImageInfo::getDescriptors() {
    return this->descriptors;
}

void ImageInfo::write(FileStorage& fs) const {
    fs << "descriptors" << this->descriptors;
    fs << "keypoints" << this->keypoints;
}

void ImageInfo::read(const FileNode& node) {

    vector<KeyPoint>* keypoints = new vector<KeyPoint>(0);
    FileNode kps = node["keypoints"];
    cv::read(kps, *keypoints);
    Mat descriptors;
    node["descriptors"] >> descriptors;
    this->label = label;
    this->keypoints = *keypoints;
    this->descriptors = descriptors.clone();
}

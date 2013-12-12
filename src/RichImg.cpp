#include <RichImg.hpp>

using namespace Anakin;

RichImg::RichImg(Img *img, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor) {
    this->aimg = img;
    this->detector = detector;
    this->extractor = extractor;
}

RichImg::RichImg(const RichImg& other) {
    this->aimg = new Img(*(other.aimg));
    this->detector = other.detector;
    this->extractor = other.extractor;
}

RichImg* RichImg::makeNew(Img* img) {
    return new RichImg(img, this->detector, this->extractor);
}

std::vector<cv::KeyPoint> RichImg::getKeypoints() {
    if (this->keypointsCalculated) {
        return this->keypoints;
    }
    return this->getFreshKeypoints();
}

void RichImg::recalculateFeatures(std::vector<int> mask) {
    std::vector<cv::KeyPoint> newKeypoints;
    int removedKeypoints = 0;
    for (int i = 0; i < this->keypoints.size(); i++) {
        if (mask[i] != 1) {
            newKeypoints.push_back(this->keypoints[i]);
        } else {
            removedKeypoints++;
        }
    }
    this->keypoints.clear();
    for (int k = 0; k < newKeypoints.size(); k++) {
        this->keypoints.push_back(newKeypoints[k]);
    }
    this->extractor->compute(this->aimg->getGrayImg(), this->keypoints, this->descriptors);
    this->descriptorsCalculated = true;
}

std::vector<cv::KeyPoint> RichImg::getFreshKeypoints() {
    this->detector->detect(this->aimg->getGrayImg(), this->keypoints);
    this->keypointsCalculated = true;
    return this->keypoints;
}

cv::Mat RichImg::getDescriptors() {
    if (!this->keypointsCalculated) {
        this->getFreshKeypoints();
    }
    if (this->descriptorsCalculated) {
        return this->descriptors;
    }
    return this->getFreshDescriptors();
}

cv::Mat RichImg::getFreshDescriptors() {
    if (!this->keypointsCalculated) {
        this->getFreshKeypoints();
    }
    this->extractor->compute(this->aimg->getGrayImg(), this->keypoints, this->descriptors);
    this->descriptorsCalculated = true;
    return this->descriptors;
}

Img* RichImg::getImage() {
    return this->aimg;
}

RichImg::~RichImg() {
    delete this->aimg;
}

#include <RichImg.hpp>

using namespace Anakin;

RichImg::RichImg(Img *img, cv::Ptr<cv::FeatureDetector>& detector, cv::Ptr<cv::DescriptorExtractor>& extractor) {
    this->aimg = img;
    this->detector = detector;
    this->extractor = extractor;
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

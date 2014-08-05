#include "data/RichImg.hpp"

using namespace Anakin;

RichImg::RichImg(Img *img, cv::Ptr<cv::FeatureDetector>& detector,
		cv::Ptr<cv::DescriptorExtractor>& extractor) {
	keypoints= new std::vector<cv::KeyPoint> ();
	descriptors= new cv::Mat();
	this->aimg = img;
	this->detector = detector;
	this->extractor = extractor;
}

RichImg::RichImg(ImageInfo* imgInfo) {
	cv::Mat img = cv::Mat();
	this->aimg = new Img(img, imgInfo->getLabel());
	this->keypoints = imgInfo->getKeypoints();
	this->descriptors = imgInfo->getDescriptors();
	this->constructedWithImageInfo = true;
	this->descriptorsCalculated = true;
	this->keypointsCalculated = true;
}

RichImg::RichImg(const RichImg& other) {
	this->aimg = new Img(*(other.aimg));
	if (other.constructedWithImageInfo) {
		this->constructedWithImageInfo = true;
		this->aimg = other.aimg;
		this->keypoints = other.keypoints;
		this->descriptors = other.descriptors;
	} else {
		this->detector = other.detector;
		this->extractor = other.extractor;
	}
}

RichImg* RichImg::makeNew(Img* img) {
	return new RichImg(img, this->detector, this->extractor);
}

std::vector<cv::KeyPoint> * RichImg::getKeypoints() {
	if (this->keypointsCalculated) {
		return this->keypoints;
	}
	return this->getFreshKeypoints();
}

void RichImg::recalculateFeatures(std::vector<int> mask) {
	if (this->constructedWithImageInfo)
		return;
	std::vector<cv::KeyPoint> * newKeypoints = new std::vector<cv::KeyPoint>();
	int removedKeypoints = 0;
	for (uint i = 0; i < this->keypoints->size(); i++) {
		if (mask[i] != 1) {
			newKeypoints->push_back(this->keypoints->at(i));
		} else {
			removedKeypoints++;
		}
	}
	this->keypoints->clear();
	for (uint k = 0; k < newKeypoints->size(); k++) {
		this->keypoints->push_back(newKeypoints->at(k));
	}
	this->extractor->compute(this->aimg->getGrayImg(), *this->keypoints,
			*this->descriptors);
	this->descriptorsCalculated = true;
}

std::vector<cv::KeyPoint> * RichImg::getFreshKeypoints() {
	if (!this->constructedWithImageInfo) {
		this->detector->detect(this->aimg->getGrayImg(), *(this->keypoints));
		this->keypointsCalculated = true;
	}
	return this->keypoints;
}

cv::Mat * RichImg::getDescriptors() {
	if (!this->keypointsCalculated) {
		this->getFreshKeypoints();
	}
	if (this->descriptorsCalculated) {
		return this->descriptors;
	}
	return this->getFreshDescriptors();
}

cv::Mat * RichImg::getFreshDescriptors() {
	if (!this->constructedWithImageInfo) {
		if (!this->keypointsCalculated) {
			this->getFreshKeypoints();
		}
		this->extractor->compute(this->aimg->getGrayImg(), *this->keypoints,
				*this->descriptors);
		this->descriptorsCalculated = true;
	}
	return this->descriptors;
}

ImageInfo* RichImg::getImageInfo() {
	ImageInfo* ii = new ImageInfo(this->aimg->getLabel(), getFreshKeypoints(),
			getFreshDescriptors());
	return ii;
}

Img* RichImg::getImage() {
	return this->aimg;
}

RichImg::~RichImg() {
	delete this->aimg;
}

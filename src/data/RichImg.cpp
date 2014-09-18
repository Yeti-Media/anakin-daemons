#include "data/RichImg.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

RichImg::RichImg(const Ptr<Img> & img, const Ptr<FeatureDetector>& detector,
		const Ptr<DescriptorExtractor>& extractor) {
	keypoints = makePtr<vector<KeyPoint>>();
	descriptors = makePtr<Mat>();
	this->aimg = img;
	this->detector = detector;
	this->extractor = extractor;
}

RichImg::RichImg(const Ptr<ImageInfo> & imgInfo) {
	Ptr<Mat> img = makePtr<Mat>();
	this->aimg = makePtr<Img>(img, imgInfo->getLabel());
	this->keypoints = imgInfo->getKeypoints();
	this->descriptors = imgInfo->getDescriptors();
	this->constructedWithImageInfo = true;
	this->descriptorsCalculated = true;
	this->keypointsCalculated = true;
}

RichImg::RichImg(const Ptr<RichImg> & other) {
	if (other->constructedWithImageInfo) {
		this->constructedWithImageInfo = true;
		this->aimg = other->aimg;
		this->keypoints = other->keypoints;
		this->descriptors = other->descriptors;
	} else {
		this->aimg = makePtr<Img>(other->aimg); //TODO check if this line must be the first
		this->detector = other->detector;
		this->extractor = other->extractor;
	}
}

Ptr<RichImg> RichImg::makeNew(const Ptr<Img> & img) {
	return makePtr<RichImg>(img, this->detector, this->extractor);
}

Ptr<vector<KeyPoint>> RichImg::getKeypoints() {
	if (this->keypointsCalculated) {
		return this->keypoints;
	}
	return this->getFreshKeypoints();
}

void RichImg::recalculateFeatures(const vector<int> &  mask) {
	if (this->constructedWithImageInfo)
		return;
	//TODO optimize... KeyPoint or pointer KeyPoint?
	Ptr<vector<KeyPoint>> newKeypoints = makePtr<vector<KeyPoint>>();
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
	this->extractor->compute(*this->aimg->getGrayImg(), *this->keypoints,
			*this->descriptors);
	this->descriptorsCalculated = true;
}

Ptr<vector<KeyPoint>> RichImg::getFreshKeypoints() {
	if (!this->constructedWithImageInfo) {
		this->detector->detect(*this->aimg->getGrayImg(), *this->keypoints);
		this->keypointsCalculated = true;
	}
	return this->keypoints;
}

Ptr<Mat> RichImg::getDescriptors() {
	if (!this->keypointsCalculated) {
		this->getFreshKeypoints();
	}
	if (this->descriptorsCalculated) {
		return this->descriptors;
	}
	return this->getFreshDescriptors();
}

Ptr<Mat> RichImg::getFreshDescriptors() {
	if (!this->constructedWithImageInfo) {
		if (!this->keypointsCalculated) {
			this->getFreshKeypoints();
		}
		this->extractor->compute(*this->aimg->getGrayImg(), *this->keypoints,
				*this->descriptors);
		this->descriptorsCalculated = true;
	}
	return this->descriptors;
}

Ptr<ImageInfo> RichImg::getImageInfo() {
	return makePtr<ImageInfo>(this->aimg->getLabel(), getFreshKeypoints(),
			getFreshDescriptors());
}

Ptr<Img> RichImg::getImage() {
	return this->aimg;
}

RichImg::~RichImg() {
}

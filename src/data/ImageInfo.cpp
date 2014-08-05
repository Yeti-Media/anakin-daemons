#include "data/ImageInfo.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

ImageInfo::ImageInfo(string label, vector<KeyPoint> * keypoints,
		Mat * descriptors) {
	this->label = label;
	this->keypoints = keypoints;
	this->descriptors = descriptors;
}

ImageInfo::ImageInfo() {
	this->label = "DEFAULT";
	this->keypoints = new vector<KeyPoint>(0);
	this->descriptors = new Mat();
}

ImageInfo::~ImageInfo() {

	if (this->keypoints!=NULL) {
		delete this->keypoints; //FIXME memory leak, delete contents?
	}

	if (this->descriptors!=NULL) {
		delete this->descriptors;
	}
}

void ImageInfo::setLabel(std::string l) {
	this->label = l;
}

string ImageInfo::getLabel() {
	return this->label;
}

vector<KeyPoint> * ImageInfo::getKeypoints() {
	return this->keypoints;
}

Mat * ImageInfo::getDescriptors() {
	return this->descriptors;
}

void ImageInfo::write(FileStorage& fs) const {
	fs << "descriptors" << *this->descriptors;
	fs << "keypoints" << *this->keypoints;
}

void ImageInfo::read(const FileNode& node) {

	if (this->keypoints!=NULL) {
		delete this->keypoints;
	}

	if (this->descriptors!=NULL) {
		delete this->descriptors;
	}

	this->keypoints = new vector<KeyPoint>(0);
	this->descriptors = new Mat();

	FileNode kps = node["keypoints"];
	cv::read(kps, *this->keypoints);
	//Mat * descriptors;
	node["descriptors"] >> *this->descriptors;
	this->label = label;
	//this->descriptors = &(descriptors->clone());
}

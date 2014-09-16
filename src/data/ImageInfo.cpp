#include "data/ImageInfo.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

ImageInfo::ImageInfo(const string & label, const Ptr<vector<KeyPoint>> & keypoints,
		const Ptr<Mat> & descriptors) {
	this->label = label;
	this->keypoints = keypoints;
	this->descriptors = descriptors;
}

ImageInfo::ImageInfo() {
	this->label = "DEFAULT";
	this->keypoints = makePtr<vector<KeyPoint>>();
	this->descriptors = makePtr<Mat>();
}

ImageInfo::~ImageInfo() {

}

void ImageInfo::setLabel(std::string l) {
	this->label = l;
}

string ImageInfo::getLabel() {
	return this->label;
}

Ptr<vector<KeyPoint>> ImageInfo::getKeypoints() {
	return this->keypoints;
}

Ptr<Mat> ImageInfo::getDescriptors() {
	return this->descriptors;
}

void ImageInfo::write(FileStorage& fs) const {
	fs << "descriptors" << *this->descriptors;
	fs << "keypoints" << *this->keypoints;
}

void ImageInfo::read(const FileNode& node) {

	this->keypoints = makePtr<vector<KeyPoint>>();
	this->descriptors = makePtr<Mat>();

	FileNode kps = node["keypoints"];
	cv::read(kps, *this->keypoints);
	//Mat * descriptors;
	node["descriptors"] >> *this->descriptors;
	this->label = label;
	//this->descriptors = &(descriptors->clone());
}

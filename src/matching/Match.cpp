#include "matching/Match.hpp"

using namespace Anakin;

Match::Match(RichImg* scene, RichImg* pattern, cv::Ptr<vector<cv::DMatch>> matches,
		cv::Mat& homography, cv::Point2f center,
		const vector<cv::KeyPoint> & matchedKeypoints) {

	this->scene = scene;
	this->pattern = pattern;
	this->matches = matches;
	this->homography = homography;
	this->center = center;
	this->matchedKeypoints = matchedKeypoints;

}

Match::~Match() {
	delete pattern;
}

//FIXME cLEANUP???

RichImg* Match::getScene() {
	return this->scene;
}

RichImg* Match::getPattern() {
	return this->pattern;
}

cv::Ptr<vector<cv::DMatch>> Match::getMatches() {
	return this->matches;
}

cv::Mat& Match::getHomography() {
	return this->homography;
}

cv::Point2f Match::getCenter() {
	return this->center;
}

vector<cv::KeyPoint> Match::getMatchedKeypoints() {
	return this->matchedKeypoints;
}

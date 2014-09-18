#include "matching/Match.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

Match::Match(const Ptr<RichImg> & scene, const Ptr<RichImg> & pattern,
		const Ptr<vector<DMatch>> & matches, const Mat& homography,
		const Point2f & center, const vector<KeyPoint> & matchedKeypoints) {

	this->scene = scene;
	this->pattern = pattern;
	this->matches = matches;
	this->homography = homography;
	this->center = center;
	this->matchedKeypoints = matchedKeypoints;

}

Match::~Match() {
}

Ptr<RichImg> Match::getScene() {
	return this->scene;
}

Ptr<RichImg> Match::getPattern() {
	return this->pattern;
}

Ptr<vector<DMatch>> Match::getMatches() {
	return this->matches;
}

Mat& Match::getHomography() {
	return this->homography;
}

Point2f Match::getCenter() {
	return this->center;
}

vector<KeyPoint> Match::getMatchedKeypoints() {
	return this->matchedKeypoints;
}

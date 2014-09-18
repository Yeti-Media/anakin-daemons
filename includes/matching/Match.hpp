#ifndef ANAKINMATCH_HPP
#define ANAKINMATCH_HPP

#include "data/RichImg.hpp"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 * This class represent a match result given by a pattern detector
 */
class Match {
public:
	/**
	 * Initializer
	 * param: RichImg : the scene where a pattern was found
	 * param: RichImg : the pattern found in the scene
	 * param: vector<DMatch>& : the matches between the scene and a pattern
	 * param: Mat : homography between the scene and a pattern
	 * param: Point2f center : the center of the match
	 * param: vector<KeyPoint> matchedKeypoints : the keypoints in the scene that matched with the pattern
	 */
	Match(const Ptr<RichImg> & scene, const Ptr<RichImg> & pattern,
			const Ptr<vector<DMatch>> & matches, const Mat& homography,
			const Point2f & center, const vector<KeyPoint> & matchedKeypoints);
	virtual ~Match();

	/**
	 * return: scene image : const RichAnakinImg&
	 */
	Ptr<RichImg> getScene();

	/**
	 * return: pattern image : const RichAnakinImg&
	 */
	Ptr<RichImg> getPattern();

	/**
	 * return: matches between scene and pattern : const vector<DMatch>&
	 */
	Ptr<vector<DMatch>> getMatches();

	/**
	 * return: homography between scene and pattern : const Mat&
	 */
	Mat& getHomography();

	/**
	 * return the center in which the pattern matched in the scene
	 */
	Point2f getCenter();

	/**
	 * return the keypoints in the scene that matched with the pattern
	 */
	vector<KeyPoint> getMatchedKeypoints();

protected:
private:
	Ptr<RichImg> scene;
	Ptr<RichImg> pattern;
	Ptr<vector<DMatch>> matches;
	Point2f center;
	vector<KeyPoint> matchedKeypoints;
	Mat homography;

};

}
;

#endif // ANAKINMATCH_HPP

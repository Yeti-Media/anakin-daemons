#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <map>
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "matching/Match.hpp"
#include "processing/SFBMCache.hpp"
#include <utils/files/QuickLZ.hpp>

using namespace std;
namespace Anakin {

/**
 * This class is used to detect patterns on a scene
 * Uses SerializableFlannBasedMatcher (SFBM) which is a FlannBasedMatcher with serialization capabilities
 */
class BasicFlannDetector {
public:
	/**
	 * Constructor
	 * detector  : a SFBM that will find patterns on a scene
	 * cache     : trainers, patterns and scenarios cache used by the SFBM
	 * minRatio  :   since the detection is done by getting two best matches for every descriptor
	 *               this sets the minimum value of the distance ratio between the the best match and the better one
	 * min_matches_allowed   :   the minimum matches to consider a pattern as found
	 */
	BasicFlannDetector(SerializableFlannBasedMatcher * detector,
			SFBMCache* cache, float minRatio = 1.f / 1.5f,
			int min_matches_allowed = 8);
	/**
	 * find patterns in a scene and returns a vector with every match obtained
	 */
	vector<Match*>* findPatterns(QuickLZ* quickLZstate, RichImg* scene,
			bool * error);
	/**
	 * change the SFBM to use
	 */
	void changeMatcher(SerializableFlannBasedMatcher * matcher);
protected:
	void getMatches(const cv::Mat& queryDescriptors,
			vector<cv::DMatch>& matches);
	bool keyExist(map<int, cv::Ptr<vector<cv::DMatch>>>* m, int key);
	void getKeys(map<int, cv::Ptr<vector<cv::DMatch>>>* m,
			vector<int>* keys);
	vector<Match*>* findPatterns_usingTraining(QuickLZ* quickLZstate,
			RichImg* scene, bool * error);
private:
	float minRatio;
	int min_matches_allowed;
	SerializableFlannBasedMatcher * detector;
	SFBMCache* cache;

};

}
;
#endif // BASICFLANNDETECTOR_HPP

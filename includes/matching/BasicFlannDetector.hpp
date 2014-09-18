#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <map>
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "matching/Match.hpp"
#include "processing/SFBMCache.hpp"
#include <utils/files/QuickLZ.hpp>

using namespace std;
using namespace cv;

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
	BasicFlannDetector(Ptr<SerializableFlannBasedMatcher> detector,
			Ptr<SFBMCache> cache, float minRatio = 1.f / 1.5f,
			int min_matches_allowed = 8);

	virtual ~BasicFlannDetector();

	/**
	 * find patterns in a scene and returns a vector with every match obtained
	 */
	Ptr<vector<Ptr<Match>>> findPatterns(QuickLZ* quickLZstate, Ptr<RichImg> scene,
			bool & error);
	/**
	 * change the SFBM to use
	 */
	void changeMatcher(Ptr<SerializableFlannBasedMatcher> matcher);
protected:
	void getMatches(const Mat& queryDescriptors,
			vector<DMatch>& matches);
	bool keyExist(const Ptr<map<int, Ptr<vector<DMatch>>>> & m, int key);
	void getKeys(const Ptr<map<int, Ptr<vector<DMatch>>>> & m,
			Ptr<vector<int>> & keys);
	Ptr<vector<Ptr<Match>>> findPatterns_usingTraining(QuickLZ* quickLZstate,
			Ptr<RichImg> scene, bool & error);
private:
	float minRatio;
	int min_matches_allowed;
	Ptr<SerializableFlannBasedMatcher> detector;
	Ptr<SFBMCache> cache;

};

}
;
#endif // BASICFLANNDETECTOR_HPP

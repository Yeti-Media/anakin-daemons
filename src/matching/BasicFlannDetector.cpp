#include "matching/BasicFlannDetector.hpp"
#include <time.h> // for benchmark
#include <boost/foreach.hpp>
#define DEBUG 0

using namespace Anakin;
using namespace cv;
using namespace std;

BasicFlannDetector::BasicFlannDetector(
		Ptr<SerializableFlannBasedMatcher> detector, Ptr<SFBMCache> cache,
		float minRatio, int min_matches_allowed) {
	this->detector = detector;
	this->minRatio = minRatio;
	this->min_matches_allowed = min_matches_allowed;
	this->cache = cache;
}

BasicFlannDetector::~BasicFlannDetector() {
}

Ptr<vector<Ptr<Match>>> BasicFlannDetector::findPatterns(QuickLZ* quickLZstate,
		Ptr<RichImg> scene, bool & error) {
	return findPatterns_usingTraining(quickLZstate, scene, error);
}

void BasicFlannDetector::changeMatcher(
		Ptr<SerializableFlannBasedMatcher> matcher) {
	this->detector = matcher;
}

//PROTECTED

void BasicFlannDetector::getMatches(const Mat& queryDescriptors,
		vector<DMatch>& matches) {
	vector<vector<DMatch> > m_knnMatches;
	if (queryDescriptors.empty())
		return;
	if (queryDescriptors.type() != CV_32F) {
		queryDescriptors.convertTo(queryDescriptors, CV_32F);
	}
	this->detector->knnMatch(queryDescriptors, m_knnMatches, 2);
	for (size_t i = 0; i < m_knnMatches.size(); i++) {
		if (m_knnMatches[i].empty())
			continue;
		const DMatch& match_0 = m_knnMatches[i][0];
		const DMatch& match_1 = m_knnMatches[i][1];
		if (match_0.distance < match_1.distance * this->minRatio) {
			matches.push_back(match_0);
		}
	}
}

bool BasicFlannDetector::keyExist(const Ptr<map<int, Ptr<vector<DMatch>>> >& m, int key) {
	if (m->find(key) == m->end()) {
		return false;
	} else {
		return true;
	}
}

void BasicFlannDetector::getKeys(const Ptr<map<int, Ptr<vector<DMatch>>> >& m,
Ptr<vector<int>> & keys) {
	pair<int, Ptr<vector<DMatch>>> me; // what a map<int, int> is made of
	BOOST_FOREACH(me, *m) {
		keys->push_back(me.first);
	}
}

Ptr<vector<Ptr<Match>>> BasicFlannDetector::findPatterns_usingTraining(
		QuickLZ* quickLZstate, Ptr<RichImg> scene, bool & error) {
	//internal function, do not init *error=false
	Ptr<vector<Ptr<Match>>> result = makePtr<vector<Ptr<Match>>>();
	Ptr<vector<DMatch>> good_matches = makePtr<vector<DMatch>>();
	getMatches(*scene->getDescriptors(), *good_matches);
	Ptr<map<int, Ptr<vector<DMatch>>>> patternsMatched =
	makePtr<map<int, Ptr<vector<DMatch>>>>();
	for (uint m = 0; m < good_matches->size(); m++) {
		DMatch match = good_matches->at(m);
		Ptr<vector<DMatch>> pmatches;
		if (keyExist(patternsMatched, match.imgIdx)) {
			pmatches = patternsMatched->find(match.imgIdx)->second;
		} else {
			pmatches = makePtr<vector<DMatch>>();
			(*patternsMatched)[match.imgIdx] = pmatches;
		}
		pmatches->push_back(match);
	}
	Ptr<vector<int>> matchedImgsIdx = makePtr<vector<int>>();
	getKeys(patternsMatched, matchedImgsIdx);
	for (uint pm = 0; pm < matchedImgsIdx->size(); pm++) {
		int currentKey = matchedImgsIdx->at(pm);
		Ptr<vector<DMatch>> pattern_matches;
		if (keyExist(patternsMatched, currentKey)) {
			pattern_matches = patternsMatched->find(currentKey)->second;
		} else {
			continue;
		}
		if (pattern_matches->size() >= this->min_matches_allowed) {
			//cout << "matched img: " << currentKey << endl;
			vector<Point2f> obj_points;
			vector<Point2f> scene_points;

			int trainerID = stoi(this->detector->getID());

			Ptr<ImageInfo> pii = this->cache->loadPattern(quickLZstate, trainerID,
					currentKey, error);
			if (error) {
				cerr<< "*error " << error << endl;
				break;
			}
			//RichImg* pattern = this->patterns->at(currentKey);
			Ptr<RichImg> pattern = makePtr<RichImg>(pii);

			for (uint i = 0; i < pattern_matches->size(); i++) {
				//-- Get the keypoints from the good matches
				//obj_points.push_back( pattern->getKeypoints()[ (*pattern_matches)[i].queryIdx ].pt );
				//scene_points.push_back( scene->getKeypoints()[ (*pattern_matches)[i].trainIdx ].pt );
				obj_points.push_back(
						pattern->getKeypoints()->at(
								pattern_matches.get()->at(i).trainIdx).pt);
				scene_points.push_back(
						scene->getKeypoints()->at(
								pattern_matches.get()->at(i).queryIdx).pt);
			}

			vector<KeyPoint> matchedKeypoints;
			for (uint m = 0; m < pattern_matches->size(); m++) {
				DMatch cmatch = pattern_matches.get()->at(m);
				//matchedKeypoints->push_back(scene->getKeypoints()[cmatch.trainIdx]);
				matchedKeypoints.push_back(
						scene->getKeypoints()->at(cmatch.queryIdx));
			}

			Point2f center(0, 0);
			for (size_t i = 0; i < matchedKeypoints.size(); i++) {
				KeyPoint currentPoint = matchedKeypoints[i];
				center.x += currentPoint.pt.x;
				center.y += currentPoint.pt.y;
			}
			center.x /= matchedKeypoints.size();
			center.y /= matchedKeypoints.size();

			Mat H = findHomography(obj_points, scene_points, FM_RANSAC);
			result->push_back(
					makePtr<Match>(scene, pattern, pattern_matches, H, center,
							matchedKeypoints));
		}
	}
	return result;
}

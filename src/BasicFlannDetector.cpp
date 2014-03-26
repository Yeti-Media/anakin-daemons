#include "BasicFlannDetector.hpp"
#include <time.h> // for benchmark
#include <boost/foreach.hpp>
#define DEBUG 0


using namespace Anakin;
using namespace cv;
using namespace std;


    BasicFlannDetector::BasicFlannDetector( cv::Ptr<SerializableFlannBasedMatcher>  detector, SFBMCache* cache, float minRatio, int min_matches_allowed) {
        this->detector = detector;
        this->minRatio = minRatio;
        this->min_matches_allowed = min_matches_allowed;
        this->cache = cache;
    }


    std::vector<Match>* BasicFlannDetector::findPatterns(RichImg* scene) {
        return findPatterns_usingTraining(scene);
    }

    void BasicFlannDetector::changeMatcher(cv::Ptr<SerializableFlannBasedMatcher> matcher) {
        this->detector = matcher;
    }

    //PROTECTED

    void BasicFlannDetector::getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches) {
        std::vector< std::vector<cv::DMatch> > m_knnMatches;
        if (queryDescriptors.empty()) return;
        if(queryDescriptors.type()!=CV_32F) {
            queryDescriptors.convertTo(queryDescriptors, CV_32F);
        }
        ((cv::Ptr<SerializableFlannBasedMatcher>)this->detector)->knnMatch(queryDescriptors, m_knnMatches, 2);
        for (size_t i=0; i<m_knnMatches.size(); i++) {
            if (m_knnMatches[i].empty()) continue;
                const cv::DMatch& match_0 = m_knnMatches[i][0];
                const cv::DMatch& match_1 = m_knnMatches[i][1];
                if (match_0.distance < match_1.distance * this->minRatio) {
                    matches.push_back(match_0);
                }
        }
    }

    bool BasicFlannDetector::keyExist(map<int, vector<DMatch>*>* m, int key) {
        if ( m->find(key) == m->end() ) {
            return false;
        } else {
            return true;
        }
    }

    void BasicFlannDetector::getKeys(map<int, vector<DMatch>*>* m, vector<int>* keys) {
        pair<int,vector<DMatch>*> me; // what a map<int, int> is made of
        BOOST_FOREACH(me, *m) {
            keys->push_back(me.first);
        }
    }

    vector<Anakin::Match>* BasicFlannDetector::findPatterns_usingTraining(Anakin::RichImg* scene) {
        vector<Match>* result = new std::vector<Match>();
        vector<DMatch>* good_matches = new vector<DMatch>(0);
        getMatches(scene->getDescriptors(), *good_matches);
        map<int, vector<DMatch>*>* patternsMatched = new map<int, vector<DMatch>*>();
        for (uint m = 0; m < good_matches->size(); m++) {
            DMatch match = good_matches->at(m);
            vector<DMatch>* pmatches;
            if (keyExist(patternsMatched, match.imgIdx)) {
                pmatches = patternsMatched->find(match.imgIdx)->second;
            } else {
                pmatches = new vector<DMatch>(0);
                (*patternsMatched)[match.imgIdx] = pmatches;
            }
            pmatches->push_back(match);
        }
        vector<int>* matchedImgsIdx = new vector<int>(0);
        getKeys(patternsMatched, matchedImgsIdx);
        for (uint pm = 0; pm < matchedImgsIdx->size(); pm++) {
            int currentKey = matchedImgsIdx->at(pm);
            vector<DMatch>* pattern_matches;
            if (keyExist(patternsMatched, currentKey)) {
                pattern_matches = patternsMatched->find(currentKey)->second;
            } else {
                continue;
            }
            if (pattern_matches->size() >= this->min_matches_allowed) {
                //std::cout << "matched img: " << currentKey << std::endl;
                std::vector<Point2f> obj_points;
                std::vector<Point2f> scene_points;

                int trainerID = std::stoi(this->detector->getID());
                ImageInfo* pii = this->cache->loadPattern(trainerID, currentKey);
                //RichImg* pattern = this->patterns->at(currentKey);
                RichImg* pattern = new RichImg(pii);

                for( uint i = 0; i < pattern_matches->size(); i++ ) {
                    //-- Get the keypoints from the good matches
                    //obj_points.push_back( pattern->getKeypoints()[ (*pattern_matches)[i].queryIdx ].pt );
                    //scene_points.push_back( scene->getKeypoints()[ (*pattern_matches)[i].trainIdx ].pt );
                    obj_points.push_back( pattern->getKeypoints()[ (*pattern_matches)[i].trainIdx ].pt );
                    scene_points.push_back( scene->getKeypoints()[ (*pattern_matches)[i].queryIdx ].pt );
                }

                std::vector<cv::KeyPoint>* matchedKeypoints = new std::vector<cv::KeyPoint>(0);
                for (uint m = 0; m < pattern_matches->size(); m++) {
                    DMatch cmatch = (*pattern_matches)[m];
                    //matchedKeypoints->push_back(scene->getKeypoints()[cmatch.trainIdx]);
                    matchedKeypoints->push_back(scene->getKeypoints()[cmatch.queryIdx]);
                }

                cv::Point2f center(0,0);
                for ( size_t i=0; i<matchedKeypoints->size(); i++ ) {
                    cv::KeyPoint currentPoint = (*matchedKeypoints)[i];
                    center.x += currentPoint.pt.x;
                    center.y += currentPoint.pt.y;
                }
                center.x /= matchedKeypoints->size();
                center.y /= matchedKeypoints->size();

                Mat H = findHomography( obj_points, scene_points, CV_RANSAC );
                Match* pmatch = new Match(scene, pattern, pattern_matches, H, center, *matchedKeypoints);
                result->push_back(*pmatch);
            }
        }
        return result;
    }

#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <map>
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "matching/Match.hpp"
#include "processing/SFBMCache.hpp"

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
        BasicFlannDetector( cv::Ptr<SerializableFlannBasedMatcher>  detector, SFBMCache* cache, float minRatio = 1.f / 1.5f, int min_matches_allowed = 8);
        /**
        * find patterns in a scene and returns a vector with every match obtained
        */
        std::vector<Match>* findPatterns(RichImg* scene);
        /**
        * change the SFBM to use
        */
        void changeMatcher(cv::Ptr<SerializableFlannBasedMatcher> matcher);
    protected:
        void getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches);
        bool keyExist(std::map<int, std::vector<cv::DMatch>*>* m, int key);
        void getKeys(std::map<int, std::vector<cv::DMatch>*>* m, std::vector<int>* keys);
        std::vector<Match>* findPatterns_usingTraining(RichImg* scene);
    private:
        float minRatio;
        int min_matches_allowed;
        cv::Ptr<SerializableFlannBasedMatcher>  detector;
        SFBMCache* cache;

};

};
#endif // BASICFLANNDETECTOR_HPP

#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <map>
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "matching/Match.hpp"
#include "processing/SFBMCache.hpp"

namespace Anakin {

class BasicFlannDetector {
    public:
        BasicFlannDetector( cv::Ptr<SerializableFlannBasedMatcher>  detector, SFBMCache* cache, float minRatio = 1.f / 1.5f, int min_matches_allowed = 8);
        std::vector<Match>* findPatterns(RichImg* scene);
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

#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <Detector.hpp>
#include <map>
#include "SerializableFlannBasedMatcher.hpp"

namespace Anakin {

class BasicFlannDetector : public Detector {
    public:
        BasicFlannDetector( cv::Ptr<SerializableFlannBasedMatcher/*cv::DescriptorMatcher*/>  detector,  std::vector<Anakin::RichImg*>& patterns, bool train=false, float minRatio = 1.f / 1.5f, int min_matches_allowed = 8);
    protected:
        virtual std::vector<Anakin::Match>* findPatterns(Anakin::RichImg* scene);
        virtual void init();
    protected:
        virtual bool findPattern(Anakin::RichImg* scene, Anakin::RichImg* pattern, Anakin::Match** match, std::vector<bool>* mask);
        virtual void cleanMatchedKeypoints(Anakin::Match* match);
        virtual void cleanMatchedKeypoints(Anakin::Match* match, std::vector<bool>* mask);
        virtual bool refineMatchesWithHomography(
                                const std::vector<cv::KeyPoint>& queryKeypoints,
                                const std::vector<cv::KeyPoint>& trainKeypoints,
                                float reprojectionThreshold,
                                std::vector<cv::DMatch>& matches,
                                cv::Mat& homography);
        virtual void getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches);
        virtual void getMatches(const cv::Mat& patternDescriptors, const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches, std::vector<bool>* mask);
        cv::Ptr<cv::DescriptorMatcher>  detector;
        bool keyExist(std::map<int, std::vector<cv::DMatch>*>* m, int key);
        void getKeys(std::map<int, std::vector<cv::DMatch>*>* m, std::vector<int>* keys);
        std::vector<Anakin::RichImg*>* patterns;
        virtual std::vector<Anakin::Match>* findPatterns_usingTraining(Anakin::RichImg* scene);
    private:
        float minRatio;
        int min_matches_allowed;
        bool train = false;

};

};
#endif // BASICFLANNDETECTOR_HPP

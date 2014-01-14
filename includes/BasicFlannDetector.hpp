#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <Detector.hpp>

namespace Anakin {

class BasicFlannDetector : public Detector {
    public:
        BasicFlannDetector( cv::Ptr<cv::DescriptorMatcher>  detector,  std::vector<Anakin::RichImg*>& patterns, float minRatio, int min_matches_allowed);
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
        virtual void getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches, std::vector<bool>* mask);
        virtual void getMatches(const cv::Mat& patternDescriptors, const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches, std::vector<bool>* mask);
        cv::Ptr<cv::DescriptorMatcher>  detector;
        std::vector<Anakin::RichImg*>* patterns;
    private:
        float minRatio;
        int min_matches_allowed;

};

};
#endif // BASICFLANNDETECTOR_HPP

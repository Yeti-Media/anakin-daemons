#ifndef BASICFLANNDETECTOR_HPP
#define BASICFLANNDETECTOR_HPP

#include <Detector.hpp>

namespace Anakin {

class BasicFlannDetector : public Detector {
    public:
        BasicFlannDetector( cv::Ptr<cv::DescriptorMatcher>  detector,  std::vector<Anakin::RichImg>& patterns);
    protected:
        virtual std::vector<Anakin::Match>* findPatterns(Anakin::RichImg* scene);
        virtual void init();
    protected:
        virtual bool findPattern(Anakin::RichImg* scene, Anakin::RichImg* pattern, Anakin::Match** match);
        virtual void cleanMatchedKeypoints(Anakin::Match* match);
        virtual bool refineMatchesWithHomography(
                                const std::vector<cv::KeyPoint>& queryKeypoints,
                                const std::vector<cv::KeyPoint>& trainKeypoints,
                                float reprojectionThreshold,
                                std::vector<cv::DMatch>& matches,
                                cv::Mat& homography);
        virtual void getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches);
        virtual void getMatches(const cv::Mat& patternDescriptors, const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches);
        cv::Ptr<cv::DescriptorMatcher>  detector;
        std::vector<Anakin::RichImg>* patterns;
    private:

};

};
#endif // BASICFLANNDETECTOR_HPP

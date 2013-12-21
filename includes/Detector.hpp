#ifndef ANAKINDETECTOR_HPP
#define ANAKINDETECTOR_HPP

#include <RichImg.hpp>
#include <Match.hpp>

namespace Anakin {

/**
*   This class is responsible for finding matches in a scene
*
*/
class Detector
{
    public:
        /**
        * Initializer:
        * param: detector : detectors used to find matches in a scene : cv::Ptr<cv::DescriptorMatcher>
        * param: patterns : patterns to search : std::vector<RichAnakinImg>
        *
        */
        Detector( cv::Ptr<cv::DescriptorMatcher>  detector,  std::vector<Anakin::RichImg>& patterns);

        /**
        * param: scene : the image in which patterns will be searched : RichAnakinImg
        * return: matches in <scene>, each match represents a pattern found in <scene> : std::vector<AnakinMatch>
        */
        virtual std::vector<Anakin::Match>* findPatterns(Anakin::RichImg* scene) = 0;
        virtual void init() = 0;
    protected:
        virtual bool findPattern(Anakin::RichImg* scene, Anakin::RichImg* pattern, Anakin::Match** match) = 0;
        virtual void cleanMatchedKeypoints(Anakin::Match* match) = 0;
        virtual bool refineMatchesWithHomography(
                                const std::vector<cv::KeyPoint>& queryKeypoints,
                                const std::vector<cv::KeyPoint>& trainKeypoints,
                                float reprojectionThreshold,
                                std::vector<cv::DMatch>& matches,
                                cv::Mat& homography) = 0;
        virtual void getMatches(const cv::Mat& patternDescriptors,
                                const cv::Mat& queryDescriptors,
                                std::vector<cv::DMatch>& matches) = 0;
        Detector();
        cv::Ptr<cv::DescriptorMatcher>  detector;
        std::vector<Anakin::RichImg>* patterns;
    private:

};

};

#endif // ANAKINDETECTOR_HPP

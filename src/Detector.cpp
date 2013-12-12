#include <opencv2/opencv.hpp>
#include <Detector.hpp>

#define HOMOGRAPHY_REPROJECTION_THRESHOLD 3.0f
#define MIN_MATCHES_ALLOWED 8
#define DEBUG 1


using namespace Anakin;

Detector::Detector(cv::Ptr<cv::DescriptorMatcher> detector,  std::vector<RichImg>& patterns){
    this->detector = detector;
    this->patterns = &patterns;
}

Detector::Detector() {}

#include <Match.hpp>

using namespace Anakin;

Match::Match(   RichImg* scene,
                RichImg* pattern,
                std::vector<cv::DMatch>* matches,
                cv::Mat& homography) {

    this->scene = scene;
    this->pattern = pattern;
    this->matches = matches;
    this->homography = homography;

}

Anakin::RichImg* Match::getScene() {
    return this->scene;
}

Anakin::RichImg* Match::getPattern() {
    return this->pattern;
}

std::vector<cv::DMatch>* Match::getMatches() {
    return this->matches;
}

cv::Mat& Match::getHomography() {
    return this->homography;
}

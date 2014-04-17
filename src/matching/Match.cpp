#include "matching/Match.hpp"

using namespace Anakin;

Match::Match(   RichImg* scene,
                RichImg* pattern,
                std::vector<cv::DMatch>* matches,
                cv::Mat& homography,cv::Point2f center,
                std::vector<cv::KeyPoint> matchedKeypoints)
{

    this->scene = scene;
    this->pattern = pattern;
    this->matches = matches;
    this->homography = homography;
    this->center = center;
    this->matchedKeypoints = matchedKeypoints;

}

RichImg* Match::getScene()
{
    return this->scene;
}

RichImg* Match::getPattern()
{
    return this->pattern;
}

std::vector<cv::DMatch>* Match::getMatches()
{
    return this->matches;
}

cv::Mat& Match::getHomography()
{
    return this->homography;
}

cv::Point2f Match::getCenter()
{
    return this->center;
}

std::vector<cv::KeyPoint> Match::getMatchedKeypoints()
{
    return this->matchedKeypoints;
}

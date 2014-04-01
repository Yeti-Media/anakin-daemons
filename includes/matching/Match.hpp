#ifndef ANAKINMATCH_HPP
#define ANAKINMATCH_HPP

#include "data/RichImg.hpp"
#include <opencv2/opencv.hpp>

namespace Anakin {

/**
* This class represent a match result given by a pattern detector
*/
class Match {
    public:
        /**
        * Initializer
        * param: RichImg : the scene where a pattern was found
        * param: RichImg : the pattern found in the scene
        * param: std::vector<cv::DMatch>& : the matches between the scene and a pattern
        * param: cv::Mat : homography between the scene and a pattern
        * param: cv::Point2f center : the center of the match
        * param: std::vector<cv::KeyPoint> matchedKeypoints : the keypoints in the scene that matched with the pattern
        */
        Match(  Anakin::RichImg* scene,
                Anakin::RichImg* pattern,
                std::vector<cv::DMatch>* matches,
                cv::Mat& homography,
                cv::Point2f center,
                std::vector<cv::KeyPoint> matchedKeypoints);


        /**
        * return: scene image : const RichAnakinImg&
        */
        Anakin::RichImg* getScene();

        /**
        * return: pattern image : const RichAnakinImg&
        */
        Anakin::RichImg* getPattern();

        /**
        * return: matches between scene and pattern : const std::vector<cv::DMatch>&
        */
        std::vector<cv::DMatch>* getMatches();

        /**
        * return: homography between scene and pattern : const cv::Mat&
        */
        cv::Mat& getHomography();

        /**
        * return the center in which the pattern matched in the scene
        */
        cv::Point2f getCenter();

        /**
        * return the keypoints in the scene that matched with the pattern
        */
        std::vector<cv::KeyPoint> getMatchedKeypoints();

    protected:
    private:
        Anakin::RichImg* scene;
        Anakin::RichImg* pattern;
        std::vector<cv::DMatch>* matches;
        cv::Point2f center;
        std::vector<cv::KeyPoint> matchedKeypoints;
        cv::Mat homography;

};

};

#endif // ANAKINMATCH_HPP

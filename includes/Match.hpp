#ifndef ANAKINMATCH_HPP
#define ANAKINMATCH_HPP

#include <RichImg.hpp>

namespace Anakin {

/**
* This class represent a match result given by a pattern detector
*/
class Match {
    public:
        /**
        * Initializer
        * param: Anakin::RichImg : the scene where a pattern was found
        * param: Anakin::RichImg : the pattern found in the scene
        * param: std::vector<cv::DMatch>& : the matches between the scene and a pattern
        * param: cv::Mat : homography between the scene and a pattern
        */
        Match(Anakin::RichImg* scene, Anakin::RichImg* pattern, std::vector<cv::DMatch>* matches, cv::Mat& homography);


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

    protected:
    private:
        Anakin::RichImg* scene;
        Anakin::RichImg* pattern;
        std::vector<cv::DMatch>* matches;
        cv::Mat homography;
        std::vector<cv::Point2f>  points2d; //+ I need to know what these things mean
        std::vector<cv::Point3f>  points3d; //* and for what they are used
                                            //++++++++++++++++++++++++++++++++++++++++++++

        //TODO: add the following code in a corresponding function
        // Transform contour with precise homography
        //cv::perspectiveTransform(pattern.points2d, info.points2d, info.homography);

};

};

#endif // ANAKINMATCH_HPP

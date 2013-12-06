#ifndef RICHANAKINIMG_HPP
#define RICHANAKINIMG_HPP

#include <opencv2/opencv.hpp>
#include <Img.hpp>

namespace Anakin {

class RichImg {
    public:

        /**
        *   Initializer:
        *   param: aimg : the base AnakinImg with which this object will work : AnakinImg
        *   param: detector : the features detector this object will use : cv::Ptr<cv::FeatureDetector>
        *   param: extractor : the descriptors extractor this object will use : cv::Ptr<cv::DescriptorExtractor>
        *
        */
        RichImg(  Img* img,
                  cv::Ptr<cv::FeatureDetector>& detector,
                  cv::Ptr<cv::DescriptorExtractor>& extractor
        );

        RichImg(const RichImg& other) {
            this->aimg = new Img(*(other.aimg));
            this->detector = other.detector;
            this->extractor = other.extractor;
        }


        RichImg* makeNew(Img* img);

        /**
        * result: the keypoints of the image asociated to this object : std::vector<cv::KeyPoint>
        */
        std::vector<cv::KeyPoint> getKeypoints();

        /**
        * result: re-calculated image keypoints : std::vector<cv::KeyPoint>
        */
        std::vector<cv::KeyPoint> getFreshKeypoints();

        /**
        * result: image descriptors : cv::Mat
        */
        cv::Mat getDescriptors();

        /**
        * result: re-calculated image descriptors : cv::Mat
        */
        cv::Mat getFreshDescriptors();

        Img* getImage();

        ~RichImg();

        //TODO: add functions to deal with points2d, points3d and patternInfo
        //it's not done yet because I don't know if all those fields are needed

    protected:
    private:
        Img *aimg;
        cv::Ptr<cv::FeatureDetector> detector;
        cv::Ptr<cv::DescriptorExtractor> extractor;
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        bool descriptorsCalculated = false;
        bool keypointsCalculated = false;
};

};

#endif // RICHANAKINIMG_HPP

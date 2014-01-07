#ifndef RICHANAKINIMG_HPP
#define RICHANAKINIMG_HPP

#include <opencv2/opencv.hpp>
#include <Img.hpp>

namespace Anakin {

/**
    This class is used to obtain keypoints and descriptors from an image (that's currently stored in a Img object)
*/
class RichImg {
    public:

        /**
        *   Initializer:
        *   param: aimg : the base Img with which this object will work : Img*
        *   param: detector : the features detector this object will use : cv::Ptr<cv::FeatureDetector>
        *   param: extractor : the descriptors extractor this object will use : cv::Ptr<cv::DescriptorExtractor>
        *
        */
        RichImg(  Img* img,
                  cv::Ptr<cv::FeatureDetector>& detector,
                  cv::Ptr<cv::DescriptorExtractor>& extractor
        );

        /**
        *   copy constructor
        */
        RichImg(const RichImg& other);

        /**
        *   Makes a new RichImg using the detector and extractor of this one
        *   param: img : the Img object from which a the new RichImg will be constructed
        *   returns: a pointer to a new RichImg
        */
        RichImg* makeNew(Img* img);

        /**
        * result: the keypoints of the image asociated to this object : std::vector<cv::KeyPoint>
        */
        std::vector<cv::KeyPoint> getKeypoints();

        /**
        *   this will recalculate the descriptors after deleting keypoints using an int vector mask
        *   if mask[i] == 1 then the keypoint at that index will be deleted
        *   -
        *   note: running getFreshKeypoints() or getFreshDescriptors() will ignore any previous call to this function
        */
        void recalculateFeatures(std::vector<int> mask);

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

        /**
        * result: a pointer to the Img object asociated with this object
        */
        Img* getImage();

        /**
        *   class destructor
        */
        ~RichImg();

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

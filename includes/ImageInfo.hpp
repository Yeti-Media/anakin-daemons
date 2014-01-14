#ifndef IMAGEINFO_HPP
#define IMAGEINFO_HPP

#include <opencv2/opencv.hpp>

namespace Anakin {

class ImageInfo {
    public:
        ImageInfo(std::string label, std::vector<cv::KeyPoint> keypoints, cv::Mat descriptors);
        ImageInfo();
        std::string getLabel();
        std::vector<cv::KeyPoint> getKeypoints();
        cv::Mat getDescriptors();
        virtual ~ImageInfo();
        void write(cv::FileStorage& fs) const ; //Write serialization for this class
        void read(const cv::FileNode& node);    //Read serialization for this class
    protected:
    private:
        std::string label;
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
};

};

#endif // IMAGEINFO_HPP

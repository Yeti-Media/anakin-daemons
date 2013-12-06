#ifndef ANAKINIMG_HPP_INCLUDED
#define ANAKINIMG_HPP_INCLUDED

#include <opencv2/opencv.hpp>

namespace Anakin {

class Img {

public:
    /**
    * Initializer
    * param: cv::Mat& : the image used to construct this object
    * param: bool : if the image used will be cloned
    * ---
    * note : if bool param is false any changes this object makes to cv::Mat& will reflect on the original Mat
    */
    Img(cv::Mat& image, bool clone = false);

    Img(Img& other);

    /**
    * result: this will return the image asociated to this object : cv::Mat
    * ---
    * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
    */
    cv::Mat getImage();
    /**
    * result: a clone of the image asociated to this object : cv::Mat
    */
    cv::Mat safeGetImage();

    /**
    * result: this will return the grey version of the image asociated to this object : cv::Mat
    * ---
    * note : the image is returned without cloning so any chances made to it will reflect on the one used in this object
    */
    cv::Mat getGrayImg();
    /**
    * result: a clone of the grey version of the image asociated to this object : cv::Mat
    */
    cv::Mat safeGetGrayImg();

    /**
    * result: the image size : cv::Size
    */
    cv::Size getSize();

    /**
    * result: the type of the image : int
    */
    int getType();


protected:
    cv::Mat image;      //main image
    cv::Mat grayImg;    //gray version of the image
    int type;           //main image type
    cv::Size imageSize;
    void transformToGray(const cv::Mat& image, cv::Mat& gray);

};

};

#endif // ANAKINIMG_HPP_INCLUDED

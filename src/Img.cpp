#include "Img.hpp"

using namespace Anakin;

Img::Img(cv::Mat& img, bool clone) {
    if (clone) {
        this->image = img.clone();
    } else {
        this->image = img;
    }
    this->imageSize = cv::Size(image.cols, image.rows);
    transformToGray(this->image, this->grayImg);
}

Img::Img(Img& other) : Img(other.image, true)
{
}

void Img::transformToGray(const cv::Mat& image, cv::Mat& gray) {
    if (image.channels()  == 3)
        cv::cvtColor(image, gray, CV_BGR2GRAY);
    else if (image.channels() == 4)
        cv::cvtColor(image, gray, CV_BGRA2GRAY);
    else if (image.channels() == 1)
        gray = image;
}

cv::Mat Img::getImage() {
    return this->image;
}

cv::Mat Img::safeGetImage() {
    return this->image.clone();
}

cv::Mat Img::getGrayImg() {
    return this->grayImg;
}

cv::Mat Img::safeGetGrayImg() {
    return this->grayImg.clone();
}

cv::Size Img::getSize() {
    return this->imageSize;
}

int Img::getType() {
    return this->image.type();
}

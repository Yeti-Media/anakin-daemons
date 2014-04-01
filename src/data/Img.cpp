#include "data/Img.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

string Img::getFilename (const string& str) {
    unsigned lastSeparator = str.find_last_of("/\\");
    string file = lastSeparator == string::npos ? str : str.substr(lastSeparator+1);
    unsigned lastDot = file.find_last_of(".");
    string name = lastDot == string::npos ? file : file.substr(0, lastDot);
    return name;
}

Img::Img(Mat& img, string label, bool clone) {
    if (clone) {
        this->image = img.clone();
    } else {
        this->image = img;
    }
    this->label = getFilename(label);
    this->imageSize = Size(image.cols, image.rows);
    transformToGray(this->image, this->grayImg);
}

Img::Img(Img& other) : Img(other.image, other.label, true)
{
}

void Img::transformToGray(const Mat& image, Mat& gray) {
    if (image.channels()  == 3)
        cvtColor(image, gray, CV_BGR2GRAY);
    else if (image.channels() == 4)
        cvtColor(image, gray, CV_BGRA2GRAY);
    else if (image.channels() == 1)
        gray = image.clone();
}

Mat Img::getImage() {
    return this->image;
}

Mat Img::safeGetImage() {
    return this->image.clone();
}

Mat Img::getGrayImg() {
    return this->grayImg;
}

Mat Img::safeGetGrayImg() {
    return this->grayImg.clone();
}

Size Img::getSize() {
    return this->imageSize;
}

int Img::getType() {
    return this->image.type();
}

string Img::getLabel() {
    return this->label;
}

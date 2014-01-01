#include "ImageDataInput.hpp"
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;

std::vector<cv::Mat> images(0);
std::vector<std::string> labels(0);

ImageDataInput::ImageDataInput(std::string imagesFolder) {
    this->imagesFolder = imagesFolder;
    loadImages(images);
}

bool ImageDataInput::nextInput(Anakin::Img** output) {
    if (!images.empty()) {
        cv::Mat nextMat = images.back();
        std::string label = labels.back();
        images.pop_back();
        labels.pop_back();
        Img* nextImg = new Img(nextMat, label);
        *output = nextImg;
        return true;
    }
    return false;
}

void ImageDataInput::reload() {
    loadImages(images);
}

void ImageDataInput::loadImages(std::vector<cv::Mat>& images) {
    if(fs::exists( imagesFolder )) {

        fs::directory_iterator end_itr; // default construction yields past-the-end
        for (fs::directory_iterator itr( imagesFolder ); itr != end_itr; ++itr ) {

            if (!fs::is_directory(itr->status())) {
                //std::cout << "Loading image : " << itr->path().c_str() << "\n";
                cv::Mat img = cv::imread(itr->path().c_str());
                if (!img.data) {
                    std::cout << "Error loading image : " << itr->path().c_str() << "\n";
                    exit(-1);
                }
                images.push_back(img);
                labels.push_back(itr->path().string());
            }

        }

    } else {
        std::cout << "directory : " << imagesFolder << " doesn't exist\n";
        exit(-1);
    }
}

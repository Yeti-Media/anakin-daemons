#include "ImageDataInput.hpp"
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;

std::vector<cv::Mat> images(0);
void loadImages(std::vector<cv::Mat>& images);

ImageDataInput::ImageDataInput(std::string imagesFolder) {
    this->imagesFolder = imagesFolder;
    loadImages(images);
}

bool ImageDataInput::nextInput(Anakin::Img** output) {
    if (!images.empty()) {
        cv::Mat nextMat = images.back();
        images.pop_back();
        Img* nextImg = new Img(nextMat);
        *output = nextImg;
        return true;
    }
    return false;
}

void ImageDataInput::loadImages(std::vector<cv::Mat>& images) {
    if(fs::exists( imagesFolder )) {

        fs::directory_iterator end_itr; // default construction yields past-the-end
        for (fs::directory_iterator itr( imagesFolder ); itr != end_itr; ++itr ) {

            if (!fs::is_directory(itr->status())) {
                std::cout << "Loading image : " << itr->path().c_str() << "\n";
                cv::Mat img = cv::imread(itr->path().c_str());
                if (!img.data) {
                    std::cout << "Error loading image : " << itr->path().c_str() << "\n";
                    exit(-1);
                }
                images.push_back(img);
            }

        }

    }
}

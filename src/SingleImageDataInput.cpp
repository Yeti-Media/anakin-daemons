#include "SingleImageDataInput.hpp"
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations

namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;
using namespace cv;

SingleImageDataInput::SingleImageDataInput(string pathToImage) {
    this->pathToImage = pathToImage;
}

void SingleImageDataInput::reload() {
    loaded = false;
}

bool SingleImageDataInput::nextInput(Img** output) {
    if (!loaded) {
        Mat nextMat = imread(this->pathToImage);
        if (!nextMat.data) {
            std::cout << "error reading image: " << this->pathToImage << "\n";
            exit(-1);
        }
        Img* nextImg = new Img(nextMat, this->pathToImage);
        *output = nextImg;
        loaded = true;
        return true;
    }
    return false;
}

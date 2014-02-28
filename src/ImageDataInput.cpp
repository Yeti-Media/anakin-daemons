#include "ImageDataInput.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;


ImageDataInput::ImageDataInput(std::string imagesFolder, bool loadOnDemand) {
    this->imagesFolder = imagesFolder;
    this->loadOnDemand = loadOnDemand;
    if (this->loadOnDemand) {
        initializeIterator();
    } else {
        this->images = new std::vector<cv::Mat>(0);
        this->labels = new std::vector<std::string>(0);
        loadImages(this->images);
    }
}


bool ImageDataInput::nextInput(Anakin::Img** output) {
    cv::Mat nextMat;
    std::string label;
    if (this->loadOnDemand) {
        if (nextFile(&nextMat, &label)) {
            Img* nextImg = new Img(nextMat, label);
            *output = nextImg;
            return true;
        }
    } else {
        if (!this->images->empty()) {
            nextMat = this->images->back();
            label = this->labels->back();
            this->images->pop_back();
            this->labels->pop_back();
            Img* nextImg = new Img(nextMat, label);
            *output = nextImg;
            return true;
        }
    }
    return false;
}

void ImageDataInput::reload() {
    loadImages(this->images);
}

int ImageDataInput::imagesToLoad() {
    static int files = scanFiles();
    return files;
}

//PROTECTED

void ImageDataInput::loadImages(std::vector<cv::Mat>* images) {
    if(fs::exists( imagesFolder )) {
        int files = imagesToLoad();
        this->images->resize(files);
        this->labels->resize(files);
        int idx = 0;
        fs::directory_iterator end_itr; // default construction yields past-the-end
        for (fs::directory_iterator itr( imagesFolder ); itr != end_itr; ++itr ) {
            if (!fs::is_directory(itr->status())) {
                //std::cout << "Loading image : " << itr->path().c_str() << "\n";
                cv::Mat img = cv::imread(itr->path().c_str());
                if (!img.data) {
                    std::cerr << "Error loading image : " << itr->path().c_str() << "\n";
                    exit(-1);
                }
                this->images->at(idx) = img;
                std::string label(itr->path().string());
                this->labels->at(idx) = label;
                idx++;
            }

        }

    } else {
        std::cerr << "directory : " << imagesFolder << " doesn't exist\n";
        exit(-1);
    }
}

void ImageDataInput::initializeIterator() {
    if(fs::exists( imagesFolder )) {
        this->itr = new fs::directory_iterator(imagesFolder);
    } else {
        std::cerr << "directory : " << imagesFolder << " doesn't exist\n";
        exit(-1);
    }
}

bool ImageDataInput::nextFile(cv::Mat* imat, std::string * label) {
    fs::directory_iterator di = *this->itr;
    fs::directory_iterator end_itr;
    if (di != end_itr) {
        if (!fs::is_directory(di->status())) {
            cv::Mat img = cv::imread(di->path().c_str());
            if (!img.data) {
                std::cerr << "Error loading image : " << di->path().c_str() << "\n";
                exit(-1);
            }
            *imat = img;
            *label = di->path().string();
            (*this->itr)++;
        } else {
            (*this->itr)++;
            return nextFile(imat, label);
        }
        return true;
    } else {
        return false;
    }
}

//PRIVATE

int ImageDataInput::scanFiles() {
    int files = 0;
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for (fs::directory_iterator itr( imagesFolder ); itr != end_itr; ++itr ) {
            if (!fs::is_directory(itr->status())) {
                files++;
            }
    }
    return files;
}

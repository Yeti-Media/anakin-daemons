#ifndef IMAGEDATAINPUT_H
#define IMAGEDATAINPUT_H

#include <DataInput.hpp>
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

namespace Anakin {

class ImageDataInput : public DataInput {
    public:
        ImageDataInput(std::string imagesFolder, bool loadOnDemand=false);
        bool nextInput(Anakin::Img** output);
        void reload();
        int imagesToLoad();
    protected:
        void loadImages(std::vector<cv::Mat>* images);
        void initializeIterator();
        bool nextFile(cv::Mat* imat, std::string * label);
    private:
        int scanFiles();
        std::string imagesFolder;
        std::vector<cv::Mat>* images;
        std::vector<std::string>* labels;
        fs::directory_iterator* itr;
        fs::directory_iterator end_itr;
        bool loadOnDemand = false;
        std::vector<fs::path> filePaths;
        std::vector<fs::path>::const_iterator* fileItr;
        void orderFiles();
};

};

#endif // IMAGEDATAINPUT_H

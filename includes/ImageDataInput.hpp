#ifndef IMAGEDATAINPUT_H
#define IMAGEDATAINPUT_H

#include <DataInput.hpp>
#include <opencv2/opencv.hpp>

namespace Anakin {

class ImageDataInput : public DataInput {
    public:
        ImageDataInput(std::string imagesFolder);
        bool nextInput(Anakin::Img** output);
        void reload();
    protected:
        void loadImages(std::vector<cv::Mat>* images);
    private:
        std::string imagesFolder;
        std::vector<cv::Mat>* images;
        std::vector<std::string>* labels;
};

};

#endif // IMAGEDATAINPUT_H

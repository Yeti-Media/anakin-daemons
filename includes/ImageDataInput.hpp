#ifndef IMAGEDATAINPUT_H
#define IMAGEDATAINPUT_H

#include <DataInput.hpp>

namespace Anakin {

class ImageDataInput : public DataInput {
    public:
        ImageDataInput(std::string imagesFolder);
        bool nextInput(Anakin::Img** output);
    protected:
        void loadImages(std::vector<cv::Mat>& images);
    private:
        std::string imagesFolder;
};

};

#endif // IMAGEDATAINPUT_H

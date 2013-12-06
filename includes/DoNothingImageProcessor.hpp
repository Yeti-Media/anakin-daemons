#ifndef DONOTHINGIMAGEPROCESSOR_HPP
#define DONOTHINGIMAGEPROCESSOR_HPP

#include <ImageProcessor.hpp>

namespace Anakin {

class DoNothingImageProcessor : public ImageProcessor {
    public:
        DoNothingImageProcessor();
        DoNothingImageProcessor(Anakin::Img scene, Anakin::RichImg pattern);
        void setRet(bool val);
    protected:
    private:
        bool ret = false;
};

};

#endif // DONOTHINGIMAGEPROCESSOR_HPP

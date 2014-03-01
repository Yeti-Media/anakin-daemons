#ifndef IMAGEPROCESSOR_HPP
#define IMAGEPROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <Img.hpp>
#include <Detector.hpp>

namespace Anakin {

class ImageProcessor {
    public:
        ImageProcessor(Anakin::Img& scene, Anakin::Detector* detector);
        bool match(Anakin::Match* output);
    protected:
    private:
        Anakin::RichImg* scene;
        Anakin::RichImg* pattern;
        Anakin::Detector* detector;
};

};

#endif // IMAGEPROCESSOR_HPP

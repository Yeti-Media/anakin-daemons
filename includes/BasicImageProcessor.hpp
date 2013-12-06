#ifndef BASICIMAGEPROCESSOR_HPP
#define BASICIMAGEPROCESSOR_HPP

#include <JustShowImageProcessor.hpp>

namespace Anakin {

class BasicImageProcessor : public JustShowImageProcessor {
    public:
        BasicImageProcessor(Anakin::DataInput* input, Anakin::Detector* detector, cv::Ptr<cv::FeatureDetector>& fdetector, cv::Ptr<cv::DescriptorExtractor>& dextractor);
    protected:
        virtual bool process(Anakin::Img& scene);
    private:
        cv::Ptr<cv::FeatureDetector> fdetector;
        cv::Ptr<cv::DescriptorExtractor> dextractor;
};

};

#endif // BASICIMAGEPROCESSOR_HPP

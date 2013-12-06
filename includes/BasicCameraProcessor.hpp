#ifndef BASICCAMERAPROCESSOR_HPP
#define BASICCAMERAPROCESSOR_HPP

#include <JustShowCameraProcessor.hpp>

namespace Anakin {

class BasicCameraProcessor : public JustShowCameraProcessor {
    public:
        BasicCameraProcessor(   Anakin::DataInput* input,
                                Anakin::Detector* detector,
                                cv::Ptr<cv::FeatureDetector>& fdetector,
                                cv::Ptr<cv::DescriptorExtractor>& dextractor);
    protected:
        virtual bool process(Anakin::Img& scene);
    private:
        cv::Ptr<cv::FeatureDetector> fdetector;
        cv::Ptr<cv::DescriptorExtractor> dextractor;
};

};

#endif // BASICCAMERAPROCESSOR_HPP

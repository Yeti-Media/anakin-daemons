#ifndef BASICIMAGEPROCESSOR_HPP
#define BASICIMAGEPROCESSOR_HPP

#include <JustShowImageProcessor.hpp>
#include <JSON.h>
#include <JSONValue.h>
#include <ResultWriter.hpp>

using namespace std;

namespace Anakin {

class BasicImageProcessor : public JustShowImageProcessor {
    public:
        BasicImageProcessor(Anakin::DataInput* input, Anakin::Detector* detector, cv::Ptr<cv::FeatureDetector>& fdetector, cv::Ptr<cv::DescriptorExtractor>& dextractor);
        vector<JSONValue*>* getResults();
    protected:
        virtual bool process(Anakin::Img& scene);
    private:
        cv::Ptr<cv::FeatureDetector> fdetector;
        cv::Ptr<cv::DescriptorExtractor> dextractor;
        vector<JSONValue*>* result;
        vector<JSONValue*>* sceneResult;
        ResultWriter* rw;
};

};

#endif // BASICIMAGEPROCESSOR_HPP

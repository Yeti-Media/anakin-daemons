#include <opencv2/opencv.hpp>
#include "BasicImageProcessor.hpp"
#include <Match.hpp>
#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

BasicImageProcessor::BasicImageProcessor(   DataInput* input,
                                            Detector* detector,
                                            Ptr<FeatureDetector>& fdetector,
                                            Ptr<DescriptorExtractor>& dextractor)
: JustShowImageProcessor(input, detector)
{
    this->fdetector = fdetector;
    this->dextractor = dextractor;
    this->result = new vector<JSONValue*>(0);
    this->sceneResult = new vector<JSONValue*>(0);
}

vector<JSONValue*>* BasicImageProcessor::getResults() {
    return this->result;
}

bool BasicImageProcessor::process(Img& scene) {
    RichImg* scenario = new RichImg(&scene, this->fdetector, this->dextractor);
    vector<Match>* matches = this->detector->findPatterns(scenario);
    for (int m = 0; m < matches->size(); m++) {
        Match match = (*matches)[m];
        this->sceneResult->push_back(resultAsJSONValue(match.getCenter(), match.getPattern()->getImage()->getLabel(), match.getMatchedKeypoints()));
    }
    if (!matches->empty()) {
        this->result->push_back(resultAsJSONValue(scene.getLabel(), *this->sceneResult));
        this->sceneResult->clear();
    }
    return true;
}

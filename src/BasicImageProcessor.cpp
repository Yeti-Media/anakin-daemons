#include <opencv2/opencv.hpp>
#include "BasicImageProcessor.hpp"
#include <Match.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

BasicImageProcessor::BasicImageProcessor(   DataInput* input,
                                            Detector* detector,
                                            Ptr<FeatureDetector>& fdetector,
                                            Ptr<DescriptorExtractor>& dextractor,
                                            bool show)
: JustShowImageProcessor(input, detector)
{
    this->fdetector = fdetector;
    this->dextractor = dextractor;
    this->result = new vector<JSONValue*>(0);
    this->sceneResult = new vector<JSONValue*>(0);
    this->rw = new ResultWriter();
    this->show = show;
}

vector<JSONValue*>* BasicImageProcessor::getResults() {
    return this->result;
}

bool BasicImageProcessor::process(Img& scene) {
    Mat sceneMat = scene.safeGetImage();
    RichImg* scenario = new RichImg(&scene, this->fdetector, this->dextractor);
    vector<Match>* matches = this->detector->findPatterns(scenario);
    for (uint m = 0; m < matches->size(); m++) {
        Match match = (*matches)[m];
        this->sceneResult->push_back(this->rw->resultAsJSONValue(match.getCenter(), match.getPattern()->getImage()->getLabel(), match.getMatchedKeypoints()));
        if (this->show) {
            string label = match.getPattern()->getImage()->getLabel();
            int fontFace = FONT_HERSHEY_PLAIN;
            double fontScale = 1;
            int thickness = 1;
            putText(sceneMat, label, match.getCenter(), fontFace, fontScale, Scalar(0,255,0), thickness, 8);
            circle(sceneMat, match.getCenter(), 1, Scalar(0,255,0), -1, 8);
        }
    }
    if (this->show) {
        imshow("matches", sceneMat);
        waitKey();
    }
    if (!matches->empty()) {
        this->result->push_back(this->rw->resultAsJSONValue(scene.getLabel(), *this->sceneResult));
        this->sceneResult->clear();
    }
    return true;
}

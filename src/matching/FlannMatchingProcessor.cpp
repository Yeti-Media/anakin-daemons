#include "matching/FlannMatchingProcessor.hpp"

#include "matching/Match.hpp"

using namespace Anakin;

FlannMatchingProcessor::FlannMatchingProcessor(BasicFlannDetector* detector, ResultWriter* rw) {
    this->detector = detector;
    this->rw = rw;
}


std::vector<JSONValue*>* FlannMatchingProcessor::process(RichImg* scene) {
    std::vector<JSONValue*>* sceneResult = new std::vector<JSONValue*>(0);
    std::vector<Match>* matches = this->detector->findPatterns(scene);
    for (uint m = 0; m < matches->size(); m++) {
        Match match = (*matches)[m];
        sceneResult->push_back(this->rw->resultAsJSONValue(match.getCenter(), match.getPattern()->getImage()->getLabel(), match.getMatchedKeypoints()));
    }
    return sceneResult;
}

#include "matching/FlannMatchingProcessor.hpp"
#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>
#include "matching/Match.hpp"

using namespace Anakin;

FlannMatchingProcessor::FlannMatchingProcessor(BasicFlannDetector* detector) {
	this->detector = detector;
	this->cfm = new CommunicationFormatterMatchingJSON();
}

std::vector<wstring*>* FlannMatchingProcessor::process(RichImg* scene,
		bool * error) {
	//internal function, do not init *error=false
	std::vector<wstring*>* sceneResult = new std::vector<wstring*>(0);
	std::vector<Match>* matches = this->detector->findPatterns(scene, error);
	for (uint m = 0; m < matches->size() && !*error; m++) {
		Match match = (*matches)[m];
		sceneResult->push_back(
				this->cfm->outputMatch(match.getCenter(),
						match.getPattern()->getImage()->getLabel(),
						match.getMatchedKeypoints()));
	}
	return sceneResult;
}

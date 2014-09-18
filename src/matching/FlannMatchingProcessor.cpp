#include "matching/FlannMatchingProcessor.hpp"
#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>
#include "matching/Match.hpp"
#include <utils/ClearVector.hpp>

using namespace Anakin;
using namespace std;
using namespace cv;

FlannMatchingProcessor::FlannMatchingProcessor(const Ptr<BasicFlannDetector> & detector) {
	this->detector = detector;
	this->cfm = new CommunicationFormatterMatchingJSON();
}

FlannMatchingProcessor::~FlannMatchingProcessor() {
	delete this->cfm;
}

Ptr<vector<Ptr<wstring>>> FlannMatchingProcessor::process(QuickLZ* quickLZstate, const Ptr<RichImg> & scene,
			bool & error) {
	Ptr<vector<Ptr<wstring>>> sceneResult = makePtr<vector<Ptr<wstring>>>(0);
	Ptr<vector<Ptr<Match>>> matches = detector->findPatterns(quickLZstate,
			scene, error);
	for (uint m = 0; m < matches->size() && !error; m++) {
		Ptr<Match> match = (*matches)[m];
		sceneResult->push_back(
				this->cfm->outputMatch(match->getCenter(),
						match->getPattern()->getImage()->getLabel(),
						match->getMatchedKeypoints()));
	}
	return sceneResult;
}

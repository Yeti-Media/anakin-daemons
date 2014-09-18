#include <opencv2/opencv.hpp>
#include "processing/Trainer.hpp"

#define DEBUG 1

using namespace std;
using namespace Anakin;

Trainer::Trainer(const Ptr<FlannBasedMatcher> & detector,
		const Ptr<vector<Ptr<RichImg>>>& patterns,
const string & outputFolder, const string & fileName) {
	this->detector = detector;
	this->patterns = patterns;
	this->outputFolder = outputFolder;
	this->fileName = fileName;
}

Trainer::~Trainer() {

}


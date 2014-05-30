#include <opencv2/opencv.hpp>
#include "processing/Trainer.hpp"

#define DEBUG 1

using namespace std;
using namespace Anakin;

Trainer::Trainer(cv::Ptr<cv::DescriptorMatcher> detector,
		vector<RichImg*>& patterns, string outputFolder,
		string fileName) {
	this->detector = detector;
	this->patterns = &patterns;
	this->outputFolder = outputFolder;
	this->fileName = fileName;
}

Trainer::~Trainer() {

}


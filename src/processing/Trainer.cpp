#include <opencv2/opencv.hpp>
#include "processing/Trainer.hpp"

#define DEBUG 1

using namespace Anakin;

Trainer::Trainer(cv::Ptr<cv::DescriptorMatcher> detector,
		std::vector<RichImg*>& patterns, std::string outputFolder,
		std::string fileName) {
	this->detector = detector;
	this->patterns = &patterns;
	this->outputFolder = outputFolder;
	this->fileName = fileName;
}

Trainer::~Trainer() {

}


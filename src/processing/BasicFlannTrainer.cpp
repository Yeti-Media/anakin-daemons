#include "processing/BasicFlannTrainer.hpp"
#include <time.h> // for benchmark
#define DEBUG 0

using namespace Anakin;
using namespace cv;
using namespace std;

void BasicFlannTrainer::train_and_save() {
	std::vector<cv::Mat> pdescriptors(this->patterns->size());
	for (uint i = 0; i < this->patterns->size(); i++) {
		RichImg* p = this->patterns->at(i);
		pdescriptors.at(i) = p->getDescriptors().clone();
	}
	((Ptr<SerializableFlannBasedMatcher> ) this->detector)->train(pdescriptors);
	//this->detector->add(pdescriptors);
	//this->detector->train();
	((Ptr<SerializableFlannBasedMatcher> ) this->detector)->save(
			this->outputFolder + this->fileName);
}

BasicFlannTrainer::BasicFlannTrainer(
		cv::Ptr<SerializableFlannBasedMatcher> detector,
		std::vector<Anakin::RichImg*>& patterns, std::string outputFolder,
		std::string fileName) :
		Trainer(detector, patterns, outputFolder, fileName) {
}
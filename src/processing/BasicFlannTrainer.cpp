#include <processing/BasicFlannTrainer.hpp>

//#include <time.h> // for benchmark

using namespace Anakin;
using namespace std;

void BasicFlannTrainer::train_and_save() {
	vector<cv::Mat> pdescriptors(this->patterns->size());
	for (uint i = 0; i < this->patterns->size(); i++) {
		RichImg* p = this->patterns->at(i);
		pdescriptors.at(i) = p->getDescriptors().clone();
	}
	((cv::Ptr<SerializableFlannBasedMatcher>) this->detector)->train(
			pdescriptors);
	//this->detector->add(pdescriptors);
	//this->detector->train();
	((cv::Ptr<SerializableFlannBasedMatcher>) this->detector)->save(
			this->outputFolder + this->fileName);
}

BasicFlannTrainer::BasicFlannTrainer(
		cv::Ptr<SerializableFlannBasedMatcher> detector,
		vector<Anakin::RichImg*>& patterns, string outputFolder,
		string fileName) :
		Trainer(detector, patterns, outputFolder, fileName) {
}

BasicFlannTrainer::~BasicFlannTrainer() {

}

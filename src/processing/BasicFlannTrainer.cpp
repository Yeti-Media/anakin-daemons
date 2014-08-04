#include <processing/BasicFlannTrainer.hpp>

//#include <time.h> // for benchmark

using namespace Anakin;
using namespace std;

void BasicFlannTrainer::train_and_save(QuickLZ* quickLZstate) {
	vector<cv::Mat> * pdescriptors = new vector<cv::Mat>(
			this->patterns->size());
	for (uint i = 0; i < this->patterns->size(); i++) {
		RichImg* p = this->patterns->at(i);
		pdescriptors->at(i) = p->getDescriptors()->clone();
	}
	((cv::Ptr<SerializableFlannBasedMatcher>) this->detector)->train(
			pdescriptors);
	//this->detector->add(pdescriptors);
	//this->detector->train();
	((cv::Ptr<SerializableFlannBasedMatcher>) this->detector)->save(
			quickLZstate, this->outputFolder + this->fileName);
	delete pdescriptors;
}

BasicFlannTrainer::BasicFlannTrainer(
		cv::Ptr<SerializableFlannBasedMatcher> detector,
		vector<Anakin::RichImg*>& patterns, string outputFolder,
		string fileName) :
		Trainer(detector, patterns, outputFolder, fileName) {
}

BasicFlannTrainer::~BasicFlannTrainer() {

}

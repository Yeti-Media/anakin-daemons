#include <processing/BasicFlannTrainer.hpp>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/mat.inl.hpp>
#include <sys/types.h>

//#include <time.h> // for benchmark

using namespace Anakin;
using namespace std;
using namespace cv;

void BasicFlannTrainer::train_and_save(QuickLZ* quickLZstate) {
	vector<Mat> pdescriptors(this->patterns->size());
	for (uint i = 0; i < this->patterns->size(); i++) {
		Ptr<RichImg> p = this->patterns->at(i);
		pdescriptors.at(i) = p->getDescriptors()->clone();
	}
	this->detector.staticCast<SerializableFlannBasedMatcher>()->train(
			pdescriptors);
	this->detector.staticCast<SerializableFlannBasedMatcher>()->save(
			quickLZstate, this->outputFolder + this->fileName, Ptr<string>());
}

BasicFlannTrainer::BasicFlannTrainer(const Ptr<FlannBasedMatcher> & detector,
		const Ptr<vector<Ptr<RichImg>>>& patterns,
		const string & outputFolder,
		const string & fileName) :
		Trainer(detector, patterns, outputFolder, fileName) {
}

BasicFlannTrainer::~BasicFlannTrainer() {

}

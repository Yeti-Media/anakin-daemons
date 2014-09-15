#ifndef BASICFLANNTRAINER_HPP
#define BASICFLANNTRAINER_HPP

#include <opencv2/core.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <data/RichImg.hpp>
#include <vector>
#include <processing/Trainer.hpp>
#include <string>
#include <utils/files/QuickLZ.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

class BasicFlannTrainer: public Trainer {
public:
	BasicFlannTrainer(Ptr<SerializableFlannBasedMatcher> detector,
			vector<Anakin::RichImg*>& patterns, string outputFolder,
			string fileName);
	void train_and_save(QuickLZ* quickLZstate);
	virtual ~BasicFlannTrainer();
};

}
;
#endif // BASICFLANNTRAINER_HPP
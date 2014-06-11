#ifndef BASICFLANNTRAINER_HPP
#define BASICFLANNTRAINER_HPP

#include <data/RichImg.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <opencv2/core/core.hpp>
#include <processing/Trainer.hpp>
#include <string>
#include <vector>

namespace Anakin {

class BasicFlannTrainer: public Trainer {
public:
	BasicFlannTrainer(cv::Ptr<SerializableFlannBasedMatcher> detector,
			std::vector<Anakin::RichImg*>& patterns, std::string outputFolder,
			std::string fileName);
	void train_and_save();
	virtual ~BasicFlannTrainer();
};

}
;
#endif // BASICFLANNTRAINER_HPP
#ifndef BASICFLANNTRAINER_HPP
#define BASICFLANNTRAINER_HPP

#include "processing/Trainer.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"
#include <map>

namespace Anakin {

class BasicFlannTrainer : public Trainer {
    public:
        BasicFlannTrainer( cv::Ptr<SerializableFlannBasedMatcher>  detector,  std::vector<Anakin::RichImg*>& patterns, std::string outputFolder, std::string fileName);
        void train_and_save();
};

};
#endif // BASICFLANNTRAINER_HPP

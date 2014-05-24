#ifndef ANAKINTRAINER_HPP
#define ANAKINTRAINER_HPP

#include "data/RichImg.hpp"

namespace Anakin {

class Trainer {
public:

	Trainer(cv::Ptr<cv::DescriptorMatcher> detector,
			std::vector<Anakin::RichImg*>& patterns, std::string outputFolder,
			std::string fileName);

	virtual void train_and_save() = 0;
	virtual ~Trainer();
protected:
	cv::Ptr<cv::DescriptorMatcher> detector;
	std::vector<Anakin::RichImg*>* patterns;
	std::string outputFolder;
	std::string fileName;
private:

};

}
;

#endif // ANAKINTRAINER_HPP
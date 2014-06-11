#ifndef ANAKINTRAINER_HPP
#define ANAKINTRAINER_HPP

#include <data/RichImg.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <string>
#include <vector>

using namespace std;

namespace Anakin {

class Trainer {
public:

	Trainer(cv::Ptr<cv::DescriptorMatcher> detector, vector<RichImg*>& patterns,
			string outputFolder, string fileName);

	virtual void train_and_save() = 0;
	virtual ~Trainer();
protected:
	cv::Ptr<cv::DescriptorMatcher> detector;
	vector<RichImg*>* patterns;
	string outputFolder;
	string fileName;
private:

};

}
;

#endif // ANAKINTRAINER_HPP
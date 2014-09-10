#ifndef ANAKINTRAINER_HPP
#define ANAKINTRAINER_HPP

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <data/RichImg.hpp>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>

using namespace std;

namespace Anakin {

class Trainer {
public:

	Trainer(cv::Ptr<cv::FlannBasedMatcher> detector, vector<RichImg*>& patterns,
			string outputFolder, string fileName);

	virtual void train_and_save(QuickLZ* quickLZstate) = 0;
	virtual ~Trainer();
protected:
	cv::Ptr<cv::FlannBasedMatcher> detector;
	vector<RichImg*>* patterns;
	string outputFolder;
	string fileName;
private:

};

}
;

#endif // ANAKINTRAINER_HPP
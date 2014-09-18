#ifndef ANAKINTRAINER_HPP
#define ANAKINTRAINER_HPP

#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <data/RichImg.hpp>
#include <string>
#include <vector>
#include <utils/files/QuickLZ.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

class Trainer {
public:

	Trainer(const Ptr<FlannBasedMatcher> & detector,
			const Ptr<vector<Ptr<RichImg>>>&patterns,
			const string & outputFolder, const string & fileName);

			virtual void train_and_save(QuickLZ* quickLZstate) = 0;
			virtual ~Trainer();
		protected:
			Ptr<FlannBasedMatcher> detector;
			Ptr<vector<Ptr<RichImg>>> patterns;
			string outputFolder;
			string fileName;
		private:

		};

	}
	;

#endif // ANAKINTRAINER_HPP
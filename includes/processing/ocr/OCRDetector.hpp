#ifndef OCRDETECTOR_HPP
#define OCRDETECTOR_HPP

#include <opencv2/core/core.hpp>
#include <processing/commandrunner/CommandRunner.hpp>
#include <tesseract/publictypes.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace Anakin {

class OCRDetector {
public:
	OCRDetector(string imgPath, string datapath = "/usr/share/tesseract-ocr/",
			string lang = "eng", int mode = 0);
	vector<string>* detect(string & lastError);
protected:
private:
	bool init(bool loadImg = true);
	string lang;
	string datapath;
	tesseract::OcrEngineMode mode;
	string imgPath;
	cv::Mat img;
};

}

#endif // OCRDETECTOR_HPP
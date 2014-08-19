#ifndef OCRDETECTOR_HPP
#define OCRDETECTOR_HPP

#include <opencv2/core/core.hpp>
#include <tesseract/publictypes.h>
#include <string>
#include <utility>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class OCRDetector {
public:
	OCRDetector(string imgPath, string datapath = "/usr/share/tesseract-ocr/",
			string lang = "eng", int mode = 0);
	int demo();
	int basic_demo();
	vector<string>* detect(vector<pair<cv::Point*, cv::Point*>>* rectangles,
			bool show = false, int clearCount = 0);
	void clean();
	void clear();
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
;

#endif // OCRDETECTOR_HPP

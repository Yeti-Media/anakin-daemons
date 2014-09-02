#include <tesseract/baseapi.h>
#include <processing/ocr/OCRDetector.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <tesseract/resultiterator.h>
#include <sstream>

using namespace Anakin;
using namespace std;

cv::Point startingRectPoint;
cv::Point endingRectPoint;
bool drawing_box = false;

OCRDetector::OCRDetector(string imgPath, string datapath, string lang,
		int mode) {
	this->lang = lang;
	this->datapath = datapath;
	switch (mode) {
	case 0:
		this->mode = tesseract::OEM_TESSERACT_ONLY;
		break;
	case 1:
		this->mode = tesseract::OEM_CUBE_ONLY;
		break;
	case 2:
		this->mode = tesseract::OEM_TESSERACT_CUBE_COMBINED;
		break;
	case 3:
		this->mode = tesseract::OEM_DEFAULT;
		break;
	}
	this->imgPath = imgPath;
}

vector<string>* OCRDetector::detect(string & lastError) {
	//TODO change location??
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

	if (api->Init(this->datapath.c_str(), this->lang.c_str(), this->mode)) {
		lastError = "Could not initialize tesseract";
		return NULL;
	}

	this->img = cv::imread(this->imgPath);

	if (!this->img.data) {
		lastError = "Error loading image";
		return NULL;
	}

	api->SetImage((uchar*) this->img.data, this->img.size().width,
			this->img.size().height, this->img.channels(), this->img.step1());
	api->Recognize(0);
	vector<string>* result = new vector<string>();

	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			stringstream s;
			s << "word: \"" << word << "\"  \tconf: " << conf
					<< " \tBoundingBox: " << x1 << "," << y1 << "," << x2 << ","
					<< y2 << ";";
			result->push_back(s.str());
			delete[] word;
		} while (ri->Next(level));
	}

	api->Clear();
	api->End();
	delete api;
	//sleep(2);
	return result;
}

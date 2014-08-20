#include <tesseract/baseapi.h>
#include <processing/ocr/OCRDetector.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>

using namespace Anakin;
using namespace std;

tesseract::TessBaseAPI* api;
cv::Point startingRectPoint;
cv::Point endingRectPoint;
bool drawing_box = false;

OCRDetector::OCRDetector(string imgPath, string datapath, string lang,
		int mode) {
	cout << "imgPath = " << imgPath << endl << "datapath = " << datapath << endl
			<< "lang = " << lang << endl;
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

bool OCRDetector::init(bool loadImg) {
	cout << "datapath = " << datapath << endl << "lang = " << this->lang.c_str()
			<< endl;
	api = new tesseract::TessBaseAPI();
	const char * l = this->lang.c_str();
	const char * dp = this->datapath.c_str();
	if (!this->datapath.empty()) {
		const char * tp = "TESSDATA_PREFIX";
		setenv(tp, dp, 1);
	}
	if (api->Init(dp, l, this->mode)) {
		cout << "Could not initialize tesseract.\n";
		return false;
	}

	if (!loadImg)
		return true;

	this->img = cv::imread(this->imgPath);

	if (!this->img.data) {
		cout << "Error loading image.\n";
		return false;
	}

	api->SetImage((uchar*) this->img.data, this->img.size().width,
			this->img.size().height, this->img.channels(), this->img.step1());

	return true;
}

void draw_box(cv::Mat img, cv::Point startingPoint, int width, int height) {
	cv::Point p2(startingPoint.x + width, startingPoint.y + height);
	cv::rectangle(img, startingPoint, p2, cv::Scalar(0, 255, 0), 2, 8);
}

void adjustPoints(cv::Point* p1, cv::Point* p2) {
	int p1x = p1->x;
	int p1y = p1->y;
	int p2x = p2->x;
	int p2y = p2->y;
	if (p1x > p2x) {
		p1->x = p2x;
		p2->x = p1x;
	}
	if (p1y > p2y) {
		p1->y = p2y;
		p2->y = p1y;
	}
}

//Implement mouse callback
static void my_mouse_callback(int event, int x, int y, int flags, void* param) {
	cv::Mat* image = (cv::Mat*) param;

	switch (event) {

	case CV_EVENT_LBUTTONDOWN:
		drawing_box = true;
		startingRectPoint = cv::Point(x, y);
		break;

	case CV_EVENT_LBUTTONUP:
		drawing_box = false;
		endingRectPoint = cv::Point(x, y);
		adjustPoints(&startingRectPoint, &endingRectPoint);
		int width = endingRectPoint.x - startingRectPoint.x;
		int height = endingRectPoint.y - startingRectPoint.y;
		draw_box(*image, startingRectPoint, width, height);
		imshow("Image", *image);
		api->SetRectangle(startingRectPoint.x, startingRectPoint.y, width,
				height);
		char* outText = api->GetUTF8Text();
		cout << "OCR output:\n\n";
		cout << outText << "\n";
		break;
	}
}

int OCRDetector::demo() {

	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);

	if (!this->init()) {
		return -1;
	}

	cv::Mat clone = img.clone();

	cv::setMouseCallback("Image", my_mouse_callback, (void*) &clone);

	while (1) {
		imshow("Image", img);
		if (cv::waitKey(0) == 27)
			break;
		img = clone.clone();
	}

	return 0;

}

int OCRDetector::basic_demo() {
	cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);

	if (!this->init()) {
		return -1;
	}

	cv::Point p1(557, 220);
	cv::Point p2(1125, 390);
	adjustPoints(&p1, &p2);
	cv::rectangle(img, p1, p2, cv::Scalar(0, 255, 0), 2, 8);
	imshow("Image", this->img);
	cv::waitKey();
	api->SetRectangle(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);
	char* outText = api->GetUTF8Text();
	cout << "OCR output:\n\n";
	cout << outText << "\n";
	return 0;
}

vector<string>* OCRDetector::detect(
		vector<pair<cv::Point*, cv::Point*>>* rectangles, bool show,
		int clearCount) {
	vector<string>* result = new vector<string>(0);

	if (show)
		cv::namedWindow("Image", CV_WINDOW_AUTOSIZE);

	if (!this->init()) {
		return result;
	}

	if (rectangles->empty()) {
		cv::Point* p1 = new cv::Point(0, 0);
		cv::Point* p2 = new cv::Point(this->img.size().width,
				this->img.size().height);
		rectangles->push_back(pair<cv::Point*, cv::Point*>(p1, p2));
	}

	api->SetImage((uchar*) this->img.data, this->img.size().width,
			this->img.size().height, this->img.channels(), this->img.step1());
	int detections = 0;
	for (uint r = 0; r < rectangles->size(); r++) {
		cv::Mat imgToShow = img.clone();
		pair<cv::Point*, cv::Point*> p = rectangles->at(r);
		cv::Point p1 = *(p.first);
		cv::Point p2 = *(p.second);
		adjustPoints(&p1, &p2);
		draw_box(imgToShow, p1, p2.x - p1.x, p2.y - p1.y);

		string point1 = "(" + to_string(p1.x) + ", " + to_string(p1.y) + ")";
		string point2 = "(" + to_string(p2.x) + ", " + to_string(p2.y) + ")";
		string text = "[" + point1 + ", " + point2 + "]";
		int fontFace = cv::FONT_HERSHEY_PLAIN;
		double fontScale = 1;
		int thickness = 1;

		//int baseline=0;
		//Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

		cv::Point textOrg(p1.x, p1.y - 5);

		cv::putText(imgToShow, text, textOrg, fontFace, fontScale,
				cv::Scalar(100, 255, 25), thickness, 8);

		if (show)
			imshow("Image", imgToShow);

		api->SetRectangle(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

		char* outText = api->GetUTF8Text();
		result->push_back(outText);
		if (show) {
			cout << "OCR output at " << text << " :\n\n";
			cout << outText << "\n";
		}

		if (show)
			cv::waitKey();

		detections++;

		if (clearCount != 0 && clearCount == detections)
			clear();

	}

	return result;
}

void OCRDetector::clear() {
	api->Clear();
	api->SetImage((uchar*) this->img.data, this->img.size().width,
			this->img.size().height, this->img.channels(), this->img.step1());
}

void OCRDetector::clean() {
	api->Clear();

	api->End();

	delete api;
}

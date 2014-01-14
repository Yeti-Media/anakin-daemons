#include "OCRDetector.hpp"
#include <tesseract/baseapi.h>

using namespace Anakin;
using namespace cv;
using namespace std;
using namespace tesseract;


TessBaseAPI *api;

OCRDetector::OCRDetector(std::string imgPath, std::string datapath, std::string lang, int mode) {
    this->lang = lang;
    this->datapath = datapath;
    switch (mode) {
        case 0 : this->mode = OEM_TESSERACT_ONLY; break;
        case 1 : this->mode = OEM_CUBE_ONLY; break;
        case 2 : this->mode = OEM_TESSERACT_CUBE_COMBINED; break;
        case 3 : this->mode = OEM_DEFAULT; break;
    }
    this->imgPath = imgPath;
}

bool OCRDetector::init(bool loadImg) {
    api = new TessBaseAPI();
    const char * l = this->lang.c_str();
    const char * dp = this->datapath.c_str();
    if (!this->datapath.empty()) {
        const char * tp = "TESSDATA_PREFIX";
        setenv(tp, dp, 1);
    }
    if (api->Init(dp, l, this->mode))  {
        cout << "Could not initialize tesseract.\n";
        return false;
    }

    if (!loadImg) return true;

    this->img = imread(this->imgPath);

    if (!this->img.data) {
        cout << "Error loading image.\n";
        return false;
    }

    api->SetImage((uchar*)this->img.data, this->img.size().width, this->img.size().height, this->img.channels(), this->img.step1());

    return true;
}

void draw_box(cv::Mat img, cv::Point startingPoint, int width, int height) {
    Point p2(startingPoint.x+width, startingPoint.y+height);
    rectangle(img, startingPoint, p2, Scalar(0,255,0), 2, 8);
}

void adjustPoints(Point* p1, Point* p2) {
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

Point startingRectPoint;
Point endingRectPoint;

bool drawing_box = false;

//Implement mouse callback
static void my_mouse_callback( int event, int x, int y, int flags, void* param ){
	Mat* image = (Mat*) param;

	switch( event ){

		case CV_EVENT_LBUTTONDOWN:
			drawing_box = true;
			startingRectPoint = Point(x,y);
			break;

		case CV_EVENT_LBUTTONUP:
			drawing_box = false;
			endingRectPoint = Point(x,y);
			adjustPoints(&startingRectPoint, &endingRectPoint);
			int width = endingRectPoint.x - startingRectPoint.x;
			int height = endingRectPoint.y - startingRectPoint.y;
			draw_box( *image, startingRectPoint, width, height );
			imshow("Image", *image);
			api->SetRectangle(startingRectPoint.x, startingRectPoint.y, width, height);
			char* outText = api->GetUTF8Text();
			cout << "OCR output:\n\n";
            cout << outText << "\n";
			break;
	}
}

int OCRDetector::demo() {

    namedWindow("Image", CV_WINDOW_AUTOSIZE);

    if (!this->init()) {
        return -1;
    }

    Mat clone = img.clone();

    cv::setMouseCallback( "Image", my_mouse_callback, (void*) &clone);

    while( 1 ){
        imshow("Image", img);
		if( waitKey( 0 )==27 )
			break;
        img = clone.clone();
	}

    return 0;

}

int OCRDetector::basic_demo() {
    namedWindow("Image", CV_WINDOW_AUTOSIZE);

    if (!this->init()) {
        return -1;
    }

    Point p1(557, 220);
    Point p2(1125, 390);
    adjustPoints(&p1, &p2);
    rectangle(img, p1, p2, Scalar(0,255,0), 2, 8);
    imshow("Image", this->img);
    waitKey();
    api->SetRectangle(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);
    char* outText = api->GetUTF8Text();
    cout << "OCR output:\n\n";
    cout << outText << "\n";
    return 0;
}

vector<string>*  OCRDetector::detect(vector<pair<Point*, Point*>>* rectangles, bool show, int clearCount) {
    vector<string>* result = new vector<string>(0);

    if (show) namedWindow("Image", CV_WINDOW_AUTOSIZE);

    if (!this->init()) {
        return result;
    }

    if (rectangles->empty()) {
        Point* p1 = new Point(0,0);
        Point* p2 = new Point(this->img.size().width, this->img.size().height);
        rectangles->push_back(pair<Point*, Point*>(p1,p2));
    }

    api->SetImage((uchar*)this->img.data, this->img.size().width, this->img.size().height, this->img.channels(), this->img.step1());
    int detections = 0;
    for (int r = 0; r < rectangles->size(); r++) {
        Mat imgToShow = img.clone();
        pair<Point*, Point*> p = rectangles->at(r);
        Point p1 = *(p.first);
        Point p2 = *(p.second);
        adjustPoints(&p1, &p2);
        draw_box(imgToShow, p1, p2.x-p1.x, p2.y-p1.y);

        string point1 = "(" + to_string(p1.x) + ", " + to_string(p1.y) + ")";
        string point2 = "(" + to_string(p2.x) + ", " + to_string(p2.y) + ")";
        string text = "[" + point1 + ", " + point2 + "]";
        int fontFace = FONT_HERSHEY_PLAIN;
        double fontScale = 1;
        int thickness = 1;

        int baseline=0;
        Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);

        Point textOrg(p1.x, p1.y-5);

        putText(imgToShow, text, textOrg, fontFace, fontScale, Scalar(100,255,25), thickness, 8);

        if (show) imshow("Image", imgToShow);

        api->SetRectangle(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y);

        char* outText = api->GetUTF8Text();
        result->push_back(outText);
        if (show) {
            cout << "OCR output at " << text << " :\n\n";
            cout << outText << "\n";
        }

        if (show) waitKey();

        detections++;

        if (clearCount != 0 && clearCount == detections) clear();

    }


    return result;
}

void OCRDetector::clear() {
    api->Clear();
    api->SetImage((uchar*)this->img.data, this->img.size().width, this->img.size().height, this->img.channels(), this->img.step1());
}

void OCRDetector::clean() {
    api->Clear();

    api->End();

    delete api;
}

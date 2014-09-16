#include "data/Img.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

string Img::getFilename(const string& str) {
	unsigned lastSeparator = str.find_last_of("/\\");
	string file =
			lastSeparator == string::npos ? str : str.substr(lastSeparator + 1);
	unsigned lastDot = file.find_last_of(".");
	string name = lastDot == string::npos ? file : file.substr(0, lastDot);
	return name;
}

Img::Img(const Ptr<Mat> & img, const string & label, bool clone) {
	if (clone) {
		this->image = makePtr<Mat>(img->clone());
	} else {
		this->image = img;
	}
	this->label = getFilename(label);
	this->imageSize = Size(image->cols, image->rows);
	transformToGray(this->image, this->grayImg);
}

Img::Img(const Ptr<Img> & other) :
		Img(other->image, other->label, true) {
}

void Img::transformToGray(const Ptr<Mat> & image, Ptr<Mat> & gray) {
	if (image->channels() == 3)
		//FIXME memory leaks on *grey?
		cvtColor(*image, *gray, COLOR_BGR2GRAY);
	else if (image->channels() == 4)
		cvtColor(*image, *gray, COLOR_BGRA2GRAY);
	else if (image->channels() == 1)
		gray = makePtr<Mat>(image->clone());
}

Ptr<Mat> Img::getImage() {
	return this->image;
}

Ptr<Mat> Img::safeGetImage() {
	return makePtr<Mat>(image->clone());
}

Ptr<Mat> Img::getGrayImg() {
	return this->grayImg;
}

Ptr<Mat> Img::safeGetGrayImg() {
	return makePtr<Mat>(grayImg->clone());
}

Size Img::getSize() {
	return this->imageSize;
}

int Img::getType() {
	return this->image->type();
}

string Img::getLabel() {
	return this->label;
}

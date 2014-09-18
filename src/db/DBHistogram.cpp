#include "db/DBHistogram.hpp"
#include "utils/Constants.hpp"

using namespace Anakin;

using namespace std;
using namespace cv;

DBHistogram::DBHistogram(bool asFile, int id, int userID, bool isLandscape) {
	this->asFile = asFile;
	this->id = id;
	this->userID = userID;
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::DBHistogram(bool asFile, int id, bool isLandscape) {
	this->asFile = asFile;
	this->id = id;
	this->userID = 0;
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::DBHistogram(bool asFile, bool isLandscape) {
	this->asFile = asFile;
	this->id = 0;
	this->userID = 0;	//can couse problems
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::~DBHistogram() {
}

bool DBHistogram::hasFileData() {
	return asFile;
}

void DBHistogram::setColorData(const string & data) {
	this->colorData = makePtr<string>(data);
	this->mode = this->mode | Constants::COLOR;
}
void DBHistogram::setGrayData(const string & data) {
	this->grayData = makePtr<string>(data);
	this->mode = this->mode | Constants::GRAY;
}

void DBHistogram::setHSVData(const string & data) {
	this->hsvData = makePtr<string>(data);
	this->mode = this->mode | Constants::HSV;
}

Ptr<string> DBHistogram::getColorData() const {
	if (this->mode & Constants::COLOR) {
		return this->colorData;
	} else {
		return makePtr<string>("NULL");
	}
}

Ptr<string> DBHistogram::getGrayData() const {
	if (this->mode & Constants::GRAY) {
		return this->grayData;
	} else {
		return makePtr<string>("NULL");
	}
}

Ptr<string> DBHistogram::getHSVData() const {
	if (this->mode & Constants::HSV) {
		return this->hsvData;
	} else {
		return makePtr<string>("NULL");
	}
}

char DBHistogram::getMode() const {
	return this->mode;
}

int DBHistogram::getID() {
	return this->id;
}

int DBHistogram::getUserID() {
	return this->userID;
}

void DBHistogram::changeID(int id) {
	this->id = id;
}

void DBHistogram::changeUID(int user_id) {
	this->userID = user_id;
}

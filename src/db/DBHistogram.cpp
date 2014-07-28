#include "db/DBHistogram.hpp"
#include "utils/Constants.hpp"

using namespace Anakin;

using namespace std;

DBHistogram::DBHistogram(int id, int userID, bool isLandscape) {
	this->id = id;
	this->userID = userID;
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::DBHistogram(int id, bool isLandscape) {
	this->id = id;
	this->userID = 0;
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::DBHistogram(bool isLandscape) {
	this->id = 0;
	this->userID = 0;	//can couse problems
	this->mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

DBHistogram::~DBHistogram() {
	delete colorData;
	delete grayData;
	delete hsvData;
}

void DBHistogram::setColorData(std::string * data) {
	this->colorData = data;
	this->mode = this->mode | Constants::COLOR;
}
void DBHistogram::setGrayData(std::string * data) {
	this->grayData = data;
	this->mode = this->mode | Constants::GRAY;
}

void DBHistogram::setHSVData(std::string * data) {
	this->hsvData = data;
	this->mode = this->mode | Constants::HSV;
}

std::string * DBHistogram::getColorData() const {
	if (this->mode & Constants::COLOR) {
		return this->colorData;
	} else {
		return new string("NULL");
	}
}

std::string * DBHistogram::getGrayData() const {
	if (this->mode & Constants::GRAY) {
		return this->grayData;
	} else {
		return  new string("NULL");
	}
}

std::string * DBHistogram::getHSVData() const {
	if (this->mode & Constants::HSV) {
		return this->hsvData;
	} else {
		return  new string("NULL");
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

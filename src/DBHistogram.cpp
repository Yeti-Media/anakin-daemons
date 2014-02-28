#include "DBHistogram.hpp"
#include "Constants.hpp"

using namespace Anakin;

DBHistogram::DBHistogram(std::string label, bool isLandscape) {
    this->label = label;
    this->mode = isLandscape? Constants::LANDSCAPE : Constants::HISTOGRAM;
}

void DBHistogram::setColorData(std::string data) {
    this->colorData = data;
    this->mode = this->mode | Constants::COLOR;
}
void DBHistogram::setGrayData(std::string data) {
    this->grayData = data;
    this->mode = this->mode | Constants::GRAY;
}

void DBHistogram::setHSVData(std::string data) {
    this->hsvData = data;
    this->mode = this->mode | Constants::HSV;
}

std::string DBHistogram::getColorData() const {
    if (this->mode & Constants::COLOR) {
        return this->colorData;
    } else {
        return "";
    }
}

std::string DBHistogram::getGrayData() const {
    if (this->mode & Constants::GRAY) {
        return this->grayData;
    } else {
        return "";
    }
}

std::string DBHistogram::getHSVData() const {
    if (this->mode & Constants::HSV) {
        return this->hsvData;
    } else {
        return "";
    }
}

std::string DBHistogram::getLabel() const {
    return this->label;
}

char DBHistogram::getMode() const {
    return this->mode;
}

void DBHistogram::setID(int id) {
    this->id = id;
}

int DBHistogram::getID() {
    return this->id;
}

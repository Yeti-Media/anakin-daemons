#include "DBPattern.hpp"

using namespace Anakin;

DBPattern::DBPattern(std::string label, std::string data) {
    this->label = label;
    this->data = data;
    this->id = -1;
}

std::string DBPattern::getLabel() const {
    return this->label;
}

std::string DBPattern::getData() const {
    return this->data;
}

void DBPattern::setID(int id) {
    this->id = id;
}

int DBPattern::getID() {
    return this->id;
}

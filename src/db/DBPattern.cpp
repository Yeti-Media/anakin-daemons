#include "db/DBPattern.hpp"

using namespace Anakin;

DBPattern::DBPattern(int id, int userID, std::string data) {
    this->id = id;
    this->userID = userID;
    this->data = data;
}

DBPattern::DBPattern(int userID, std::string data) {
    this->userID = userID;
    this->data = data;
}

DBPattern::DBPattern(std::string data) {
    this->data = data;
}

std::string DBPattern::getData() const {
    return this->data;
}

int DBPattern::getID() {
    return this->id;
}

void DBPattern::changeID(int id) {
    this->id = id;
}

int DBPattern::getUserID() {
    return this->userID;
}

void DBPattern::changeUID(int user_id) {
    this->userID = user_id;
}

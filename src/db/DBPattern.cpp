#include "db/DBPattern.hpp"

using namespace std;
using namespace Anakin;
using namespace cv;

DBPattern::DBPattern(int id, int userID, bool asFile,
		const Ptr<string> & data) {
	this->id = id;
	this->userID = userID;
	this->asFile = asFile;
	this->data = data;
}

DBPattern::DBPattern(bool asFile, const Ptr<string> & data) {
	this->id = 0; //can couse problems
	this->userID = 0;
	this->asFile = asFile;
	this->data = data;
}

DBPattern::~DBPattern() {
}

bool DBPattern::hasFileData() {
	return asFile;
}

Ptr<string> DBPattern::getData() const {
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

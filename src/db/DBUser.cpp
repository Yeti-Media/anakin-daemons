#include <db/DBUser.hpp>
#include <utils/ClearVector.hpp>
#include <algorithm>

using namespace Anakin;
using namespace std;
using namespace cv;

DBUser::DBUser(int id) {
	this->id = id;
	this->patterns = makePtr<vector<Ptr<DBPattern>>>();
	this->histograms = makePtr<vector<Ptr<DBHistogram>>>();
	this->landscapes = makePtr<vector<Ptr<DBHistogram>>>();
}

DBUser::~DBUser() {
}

int DBUser::getID() const {
	return this->id;
}

Ptr<vector<Ptr<DBPattern>>> DBUser::getPatterns() const {
	return this->patterns;
}

void DBUser::addPattern(const Ptr<DBPattern> & p) {
	p->changeUID(this->id);
	this->patterns->push_back(p);
}

Ptr<vector<Ptr<DBHistogram>>> DBUser::getHistograms() const {
	return this->histograms;
}

void DBUser::addHistogram(const Ptr<DBHistogram> & h) {
	h->changeUID(this->id);
	this->histograms->push_back(h);
}

Ptr<vector<Ptr<DBHistogram>>> DBUser::getLandscapes() const {
	return this->landscapes;
}

void DBUser::addLandscape(const Ptr<DBHistogram> & l) {
	l->changeUID(this->id);
	this->landscapes->push_back(l);
}

#include <db/DBUser.hpp>
#include <utils/ClearVector.hpp>
#include <algorithm>

using namespace Anakin;
using namespace std;

DBUser::DBUser(int id) {
	this->id = id;
	this->patterns = new std::vector<DBPattern*>(0);
	this->histograms = new std::vector<DBHistogram*>(0);
	this->landscapes = new std::vector<DBHistogram*>(0);
}

DBUser::~DBUser() {
	//TODO revie possible design failure about access in getPatterns() and others
	for_each(patterns->begin(), patterns->end(),
			delete_pointer_element<DBPattern*>());
	delete this->patterns;
	for_each(histograms->begin(), histograms->end(),
			delete_pointer_element<DBHistogram*>());
	delete this->histograms;
	for_each(landscapes->begin(), landscapes->end(),
			delete_pointer_element<DBHistogram*>());
	delete this->landscapes;
}

int DBUser::getID() const {
	return this->id;
}

std::vector<DBPattern*>* DBUser::getPatterns() const {
	return this->patterns;
}

void DBUser::addPattern(DBPattern* p) {
	p->changeUID(this->id);
	this->patterns->push_back(p);
}

std::vector<DBHistogram*>* DBUser::getHistograms() const {
	return this->histograms;
}

void DBUser::addHistogram(DBHistogram* h) {
	h->changeUID(this->id);
	this->histograms->push_back(h);
}

std::vector<DBHistogram*>* DBUser::getLandscapes() const {
	return this->landscapes;
}

void DBUser::addLandscape(DBHistogram* l) {
	l->changeUID(this->id);
	this->landscapes->push_back(l);
}

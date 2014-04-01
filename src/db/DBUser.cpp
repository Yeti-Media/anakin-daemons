#include "db/DBUser.hpp"

using namespace Anakin;

DBUser::DBUser(int id) {
    this->id = id;
    this->patterns = new std::vector<DBPattern*>(0);
    this->histograms = new std::vector<DBHistogram*>(0);
    this->landscapes = new std::vector<DBHistogram*>(0);
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

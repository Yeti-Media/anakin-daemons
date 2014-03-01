#include "DBUser.hpp"

using namespace Anakin;

DBUser::DBUser(std::string id) {
    this->id = id;
    this->patterns = new std::vector<DBPattern*>(0);
    this->histograms = new std::vector<DBHistogram*>(0);
    this->landscapes = new std::vector<DBHistogram*>(0);
}

std::string DBUser::getID() const {
    return this->id;
}

std::vector<DBPattern*>* DBUser::getPatterns() const {
    return this->patterns;
}

void DBUser::addPattern(DBPattern* p) {
    this->patterns->push_back(p);
}

std::vector<DBHistogram*>* DBUser::getHistograms() const {
    return this->histograms;
}

void DBUser::addHistogram(DBHistogram* h) {
    this->histograms->push_back(h);
}

std::vector<DBHistogram*>* DBUser::getLandscapes() const {
    return this->landscapes;
}

void DBUser::addLandscape(DBHistogram* l) {
    this->landscapes->push_back(l);
}

#include "FaceMatch.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

FaceMatch::FaceMatch(pair<string, Rect>* main, vector<pair<string, vector<Rect>*>*>* details) {
    this->main = main;
    this->details = details;
}

pair<string, Rect>* FaceMatch::getMain() {
    return this->main;
}

vector<pair<string, vector<Rect>*>*>* FaceMatch::getDetails() {
    return this->details;
}

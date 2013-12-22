#include "HistMatch.hpp"

using namespace Anakin;

HistMatch::HistMatch(Img* scene, Img* pattern, float percentage) {
    this->scene = scene;
    this->pattern = pattern;
    this->percentage = percentage;
}

HistMatch::HistMatch(const HistMatch& other) {
    this->scene = other.scene;
    this->pattern = other.pattern;
    this->percentage = other.percentage;
}

Img* HistMatch::getScene() {
    return this->scene;
}

Img* HistMatch::getPattern() {
    return this->pattern;
}

float HistMatch::getPercentage() {
    return this->percentage;
}

HistMatch::~HistMatch() {
    delete this->scene;
    delete this->pattern;
}

#include <opencv2/opencv.hpp>
#include <Processor.hpp>

using namespace Anakin;

Processor::Processor(Anakin::DataInput* input,  Anakin::Detector* detector) {
    this->input = input;
    this->detector = detector;
    this->init();
}

void Processor::start() {
    Img* image;
    while (this->input->nextInput(&image)) {
        if (!this->process(*image)) break;
    }
}

int Processor::correctKeyValue(const int original) {
    return original;
}

void Processor::init() {}

bool Processor::process(Anakin::Img& scene) {
    return true;
}

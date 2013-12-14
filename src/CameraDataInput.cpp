#include "CameraDataInput.hpp"

using namespace Anakin;

CameraDataInput::CameraDataInput(const int& camera) {
    this->cameraID = camera;
    this->running = false;
    if (this->cap.open(camera)) {
        this->running = true;
    }
}

bool CameraDataInput::nextInput(Anakin::Img** output) {
    if (this->running) {
        cv::Mat nextMat;
        this->cap >> nextMat;
        Img* nextImg = new Img(nextMat, "CameraInput");
        *output = nextImg;
        return true;
    }
    return false;
}

void CameraDataInput::stop() {
    this->running = false;
}

const int CameraDataInput::getCameraID() {
    return this->cameraID;
}



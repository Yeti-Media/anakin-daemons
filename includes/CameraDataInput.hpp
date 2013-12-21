#ifndef CAMERADATAINPUT_HPP
#define CAMERADATAINPUT_HPP

#include <DataInput.hpp>
#include <opencv2/opencv.hpp>

namespace Anakin {

class CameraDataInput : public DataInput {
    public:
        CameraDataInput(const int& camera);
        bool nextInput(Anakin::Img** output);
        void stop();
        const int getCameraID();
    protected:
    private:
        int cameraID;
        bool running;
        cv::VideoCapture cap;
};

};

#endif // CAMERADATAINPUT_HPP

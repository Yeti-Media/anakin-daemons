#include <opencv2/opencv.hpp>
#include <JustShowCameraProcessor.hpp>

using namespace Anakin;

JustShowCameraProcessor::JustShowCameraProcessor(Anakin::DataInput* input,  Anakin::Detector* detector)
: Processor(input, detector)
{
}

int JustShowCameraProcessor::correctKeyValue(int original) {
    if (original >= -1 && original <= 255) {
        return original;
    } else {
        return original % 256;
    }
}

bool JustShowCameraProcessor::process(Anakin::Img& scene) {
    cv::imshow("Cam", scene.getImage());
    int keyCode = correctKeyValue(cv::waitKey(50));
    if (keyCode == 27 || keyCode == 'q') return false;
    return true;
}

void JustShowCameraProcessor::init() {
    cv::namedWindow("Cam", CV_WINDOW_AUTOSIZE);
}

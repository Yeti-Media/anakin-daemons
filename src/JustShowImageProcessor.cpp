#include <opencv2/opencv.hpp>
#include <JustShowImageProcessor.hpp>

using namespace Anakin;

JustShowImageProcessor::JustShowImageProcessor(DataInput* input,  Detector* detector)
: Processor(input, detector)
{
}

bool JustShowImageProcessor::process(Img& scene) {
    cv::imshow("Input", scene.getImage());
    cv::waitKey();
    return true;
}

void JustShowImageProcessor::init() {
    cv::namedWindow("Input", CV_WINDOW_AUTOSIZE);
}

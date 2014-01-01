#ifndef OCRDETECTOR_HPP
#define OCRDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <Img.hpp>

namespace Anakin {

class OCRDetector {
    public:
        OCRDetector(std::string lang, std::string imgPath);
        int demo();
        int basic_demo();
        std::vector<std::string>* detect(std::vector<std::pair<cv::Point*, cv::Point*>>* rectangles, bool show=false ,int clearCount=0);
        void clean();
        void clear();
    protected:
    private:
        bool init(bool loadImg=true);
        std::string lang;
        std::string imgPath;
        cv::Mat img;
};

};

#endif // OCRDETECTOR_HPP

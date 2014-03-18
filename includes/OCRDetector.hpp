#ifndef OCRDETECTOR_HPP
#define OCRDETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <Img.hpp>
#include "tesseract/publictypes.h"

namespace Anakin {

class OCRDetector {
    public:
        OCRDetector(std::string imgPath, std::string datapath="/usr/src/tesseract-ocr/", std::string lang="eng", int mode=0);
        int demo();
        int basic_demo();
        std::vector<std::string>* detect(std::vector<std::pair<cv::Point*, cv::Point*>>* rectangles, bool show=false ,int clearCount=0);
        void clean();
        void clear();
    protected:
    private:
        bool init(bool loadImg=true);
        std::string lang;
        std::string datapath;
        tesseract::OcrEngineMode mode;
        std::string imgPath;
        cv::Mat img;
};

};

#endif // OCRDETECTOR_HPP

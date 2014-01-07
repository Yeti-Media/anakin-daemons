#ifndef FACEMATCH_HPP
#define FACEMATCH_HPP

#include <opencv2/opencv.hpp>

namespace Anakin {

class FaceMatch {
    public:
        FaceMatch(std::pair<std::string, cv::Rect>* main, std::vector<std::pair<std::string, std::vector<cv::Rect>*>*>* details);
        std::pair<std::string, cv::Rect>* getMain();
        std::vector<std::pair<std::string, std::vector<cv::Rect>*>*>* getDetails();
    protected:
    private:
        std::pair<std::string, cv::Rect>* main;
        std::vector<std::pair<std::string, std::vector<cv::Rect>*>*>* details;
};

};

#endif // FACEMATCH_HPP

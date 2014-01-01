#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <opencv2/opencv.hpp>

namespace Anakin {

class Histogram {
    public:
        Histogram(cv::Mat hist, std::vector<int>* bins, int channels, std::string label);
        cv::Mat getHist();
        std::vector<int>* getBins();
        int getChannels();
        std::string getLabel();
    protected:
    private:
        cv::Mat hist;
        std::vector<int>* bins;
        int channels;
        std::string label;
};

};

#endif // HISTOGRAM_HPP

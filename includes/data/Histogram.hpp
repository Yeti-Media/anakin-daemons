#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <opencv2/opencv.hpp>

namespace Anakin
{

class Histogram
{
public:
    Histogram(cv::Mat hist, std::vector<int>* bins, int channels, std::string label="", bool minMax = false, bool avg = false);
    Histogram();
    cv::Mat getHist();
    std::vector<int>* getBins();
    int getChannels();
    std::string getLabel();
    bool isMinMax();
    bool hasAvg();
    void write(cv::FileStorage& fs) const ; //Write serialization for this class
    void read(const cv::FileNode& node);    //Read serialization for this class
protected:
private:
    cv::Mat hist;
    std::vector<int>* bins;
    int channels;
    std::string label;
    bool minMax;
    bool avg;
};

};

#endif // HISTOGRAM_HPP

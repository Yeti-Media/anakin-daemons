#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP
#include <Img.hpp>
#include <RichImg.hpp>
#include <Histogram.hpp>
#include <HistogramsIO.hpp>

namespace Anakin {

class HistogramComparator {
    public:
        HistogramComparator(std::vector<Anakin::RichImg*> patterns, HistogramsIO* io);
        void makeAndSaveLandscape(char mode, std::string label, bool saveToFile = true);
        void makeAndSaveHistograms(char mode, bool saveToFile = true);
        virtual ~HistogramComparator();
        const static char COLOR = 1;
        const static char GRAY = 2;
        const static char HSV = 4;
        const static char MINMAX = 8;
        const static char AVG = 16;
    protected:
    private:
        void update_minMax(cv::Mat minMaxHist, std::vector<cv::Mat>* hists, std::vector<int>* bins, std::vector<int> maxValues, int channels, bool firstPass);
        void update_average(cv::Mat minMaxHist, std::vector<int>* bins, int channels, int count);
        Histogram* createColorHistogram(Anakin::Img* img);
        Histogram* createGrayHistogram(Anakin::Img* img);
        Histogram* createHSVHistogram(Anakin::Img* img);
        std::vector<Anakin::RichImg*> patterns;
        HistogramsIO* io;
};

};

#endif // HISTOGRAMCOMPARATOR_HPP

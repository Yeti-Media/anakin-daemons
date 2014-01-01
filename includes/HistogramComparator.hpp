#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP
#include <Img.hpp>
#include <DataInput.hpp>
#include <HistMatch.hpp>
#include <RichImg.hpp>
#include <Histogram.hpp>

namespace Anakin {

class HistogramComparator {
    public:
        HistogramComparator(Anakin::DataInput* input, std::vector<Anakin::RichImg> patterns);
        std::vector<Anakin::HistMatch*>* compareHistograms(float minValue, char mode);
        std::vector<Anakin::HistMatch*>* compareHistogramsMinMax(Anakin::Histogram* histogram, float minValue, char mode, float safeOffset=0);
        Anakin::Histogram* train_minMax(char mode, std::string label, bool showHist);
        virtual ~HistogramComparator();
        const static char COLOR = 1;
        const static char GRAY = 2;
        const static char HSV = 4;
        const static char CORRELATION = 8;
        const static char INTERSECTION = 16;
        const static char MINMAX = 32;
        const static char AVG = 64;
    protected:
    private:
        void update_minMax(cv::Mat minMaxHist, std::vector<cv::Mat>* hists, std::vector<int>* bins, std::vector<int> maxValues, int channels, bool firstPass);
        void update_average(cv::Mat minMaxHist, std::vector<int>* bins, int channels, int count);
        void draw_hist(Histogram* histogram, bool minMax);
        void draw_hist(std::vector<cv::Mat>* hists, std::vector<int>* bins);
        void drawDottedLine(cv::Mat img, cv::Point start, cv::Point end, cv::Scalar color, int solidPixels=3, int space=4);
        double compareUsingColor(Anakin::Img* scene, Anakin::Img* pattern, int method);
        double compareUsingGray(Anakin::Img* scene, Anakin::Img* pattern, int method);
        double compareUsingHSV(Anakin::Img* scene, Anakin::Img* pattern, int method);
        double compHistMinMaxColor(Anakin::Histogram* histogram, Anakin::Img* scene, float safeOffset=0, bool useMinMax=true);
        double compHistMinMaxGray(Anakin::Histogram* histogram, Anakin::Img* scene, float safeOffset=0, bool useMinMax=true);
        double compHistMinMaxHSV(Anakin::Histogram* histogram, Anakin::Img* scene, float safeOffset=0, bool useMinMax=true);
        Anakin::DataInput* input;
        std::vector<Anakin::RichImg> patterns;
};

};

#endif // HISTOGRAMCOMPARATOR_HPP

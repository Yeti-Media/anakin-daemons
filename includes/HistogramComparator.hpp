#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP
#include <Img.hpp>
#include <DataInput.hpp>
#include <HistMatch.hpp>
#include <RichImg.hpp>

namespace Anakin {

class HistogramComparator {
    public:
        HistogramComparator(Anakin::DataInput* input, std::vector<Anakin::RichImg> patterns);
        std::vector<Anakin::HistMatch*>* compareHistograms(float minValue, char mode);
        cv::Mat train(Anakin::DataInput* input);
        void drawColorHistogram(cv::Mat hist);
        virtual ~HistogramComparator();
        const static char COLOR = 1;
        const static char GRAY = 2;
        const static char HSV = 4;
        const static char CORRELATION = 8;
        const static char INTERSECTION = 16;
    protected:
    private:
        double compareUsingColor(Anakin::Img* scene, Anakin::Img* pattern, int method);
        double compareUsingGray(Anakin::Img* scene, Anakin::Img* pattern, int method);
        double compareUsingHSV(Anakin::Img* scene, Anakin::Img* pattern, int method);
        Anakin::DataInput* input;
        std::vector<Anakin::RichImg> patterns;
};

};

#endif // HISTOGRAMCOMPARATOR_HPP

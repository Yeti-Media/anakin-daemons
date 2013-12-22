#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP
#include <Img.hpp>
#include <DataInput.hpp>

namespace Anakin {

class HistogramComparator {
    public:
        HistogramComparator(Anakin::Img* scene);
        double compareUsingColor(Anakin::Img* pattern, int method);
        double compareUsingGray(Anakin::Img* pattern, int method);
        double compareUsingHSV(Anakin::Img* pattern, int method);
        cv::Mat train(Anakin::DataInput* input);
        virtual ~HistogramComparator();
    protected:
        Img* scene;
    private:
};

};

#endif // HISTOGRAMCOMPARATOR_HPP

#ifndef HISTOGRAMSIO_HPP
#define HISTOGRAMSIO_HPP

#include <opencv2/opencv.hpp>
#include <Histogram.hpp>

namespace Anakin {

class HistogramsIO {
    public:
        HistogramsIO(std::string baseFolder);
        void load(const char mode);
        void save(std::vector<Histogram*>* input, const char mode);
        const static char COLOR = 1;
        const static char GRAY = 2;
        const static char HSV = 4;
        const static char LANDSCAPE = 8;
        std::vector<Histogram*>* getColorHistograms();
        std::vector<Histogram*>* getGrayHistograms();
        std::vector<Histogram*>* getHSVHistograms();
    protected:
    private:
        void load(std::string baseFolder, std::vector<Histogram*>* output);
        void save(std::string filename, Histogram* histogram);
        static void write(cv::FileStorage& fs, const std::string&, const Histogram& x);
        static void read(const cv::FileNode& node, Histogram& x, const Histogram& default_value=Histogram());
        void loadData(std::vector<Histogram*>* data, std::string folder);
        std::string baseFolder;
        std::vector<Histogram*>* colorHistograms;
        std::vector<Histogram*>* grayHistograms;
        std::vector<Histogram*>* hsvHistograms;
};
};
#endif // HISTOGRAMSIO_HPP

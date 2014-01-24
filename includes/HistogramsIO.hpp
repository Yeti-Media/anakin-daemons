#ifndef HISTOGRAMSIO_HPP
#define HISTOGRAMSIO_HPP

#include <opencv2/opencv.hpp>
#include <Histogram.hpp>
#include "DBDriver.hpp"
#include "DBUser.hpp"
#include "DBHistogram.hpp"

namespace Anakin {

class HistogramsIO {
    public:
        HistogramsIO(std::string baseFolder, bool useFolderAsUserID = false);
        void load(const char mode);
        void save(std::vector<Histogram*>* input, const char mode, bool saveToFile=true);
        const static char COLOR = 1;
        const static char GRAY = 2;
        const static char HSV = 4;
        const static char LANDSCAPE = 8;
        const static char YAML = 16;
        const static char XML = 32;
        std::vector<Histogram*>* getColorHistograms();
        std::vector<Histogram*>* getGrayHistograms();
        std::vector<Histogram*>* getHSVHistograms();
    protected:
    private:
        void load(std::string baseFolder, std::vector<Histogram*>* output, bool loadFromFile);
        void save(std::string filename, Histogram* histogram, bool saveToFile);
        static void write(cv::FileStorage& fs, const std::string&, const Histogram& x);
        static void read(const cv::FileNode& node, Histogram& x, const Histogram& default_value=Histogram());
        void loadData(std::vector<Histogram*>* data, std::string folder, bool loadFromFile);
        bool loadFromDB(bool loadLandscapes);
        bool initAndConnectDriver();
        void reportDBDriverError();
        std::string baseFolder;
        std::vector<Histogram*>* colorHistograms;
        std::vector<Histogram*>* grayHistograms;
        std::vector<Histogram*>* hsvHistograms;
        std::string db_user_id;
        bool load_from_db = false;
        DBDriver* driver;
};
};
#endif // HISTOGRAMSIO_HPP

#ifndef SERIALIZABLEFLANNBASEDMATCHER_HPP
#define SERIALIZABLEFLANNBASEDMATCHER_HPP

#include "opencv2/features2d/features2d.hpp"

namespace Anakin {

class SerializableFlannBasedMatcher : public cv::FlannBasedMatcher {
    public:
        SerializableFlannBasedMatcher(const cv::Ptr<cv::flann::IndexParams>& indexParams=new cv::flann::KDTreeIndexParams(),  const cv::Ptr<cv::flann::SearchParams>& searchParams=new cv::flann::SearchParams());
        SerializableFlannBasedMatcher(std::string filename, bool removeFileAfterLoad=false);
        void save(std::string filename, std::string * xmlData = NULL);
        void train(std::vector<cv::Mat> descriptors);
        virtual bool empty() const;
    protected:
    private:
        void load(std::string xmlData="");
        std::string filename;
        bool loadedFromFile = false;
        void saveIndex();
        void loadIndex(cv::Mat data);
        void compress(bool removeOriginal = false);
        void decompress(bool useOriginalNames = false, std::string * xmlData=NULL);
        int readFile(char ** data, std::string filename);
        void writeFile(char * data, std::string filename, int length);
};

};

#endif // SERIALIZABLEFLANNBASEDMATCHER_HPP

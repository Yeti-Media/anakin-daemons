#ifndef SERIALIZABLEFLANNBASEDMATCHER_HPP
#define SERIALIZABLEFLANNBASEDMATCHER_HPP

#include "opencv2/features2d/features2d.hpp"

namespace Anakin {

class SerializableFlannBasedMatcher : public cv::FlannBasedMatcher {
    public:
        SerializableFlannBasedMatcher(const cv::Ptr<cv::flann::IndexParams>& indexParams=new cv::flann::KDTreeIndexParams(),  const cv::Ptr<cv::flann::SearchParams>& searchParams=new cv::flann::SearchParams());
        SerializableFlannBasedMatcher(std::string filename);
        SerializableFlannBasedMatcher(std::string xmlData, std::string filename);
        void save(std::string filename, std::string * xmlData = NULL);
        virtual bool empty() const;
    protected:
    private:
        void load(std::string xmlData="");
        std::string filename;
        bool loadedFromFile = false;
        void saveIndex();
        void loadIndex(cv::Mat data);
};

};

#endif // SERIALIZABLEFLANNBASEDMATCHER_HPP

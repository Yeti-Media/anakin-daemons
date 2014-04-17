#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include "data/ImageInfo.hpp"
#include "db/DBDriver.hpp"
#include "db/DBUser.hpp"
#include "db/DBPattern.hpp"

namespace Anakin {

class SerializedPatternDataInput {
    public:
        SerializedPatternDataInput(std::string userID);
        bool nextInput(ImageInfo** output);
        void reload();
    protected:
    private:
        std::vector<ImageInfo*>* cache;
        bool loaded;
        int current;
        void loadData(std::vector<ImageInfo*>* data, std::string rawData);
        bool initAndConnectDriver();
        void reportDBDriverError();
        bool loadDataFromDB(std::vector<ImageInfo*>* data);
        static void read(const cv::FileNode& node, ImageInfo& x, const ImageInfo& default_value = ImageInfo());
        std::string userID;
        DBDriver* driver;
};
};

#endif // SERIALIZEDPATTERNDATAINPUT_HPP

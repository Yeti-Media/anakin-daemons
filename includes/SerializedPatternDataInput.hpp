#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include <ImageInfo.hpp>
#include "DBDriver.hpp"
#include "DBUser.hpp"
#include "DBPattern.hpp"

namespace Anakin {

class SerializedPatternDataInput {
    public:
        SerializedPatternDataInput(std::string inputFolder, bool useInputFolderAsUserID, bool useCache=false);
        bool nextInput(ImageInfo** output);
        void reload();
    protected:
    private:
        std::string inputFolder;
        std::vector<ImageInfo*>* cache;
        bool useCache;
        bool loaded;
        int current;
        void loadData(std::vector<ImageInfo*>* data, bool loadFromMemory=false);
        bool initAndConnectDriver();
        void reportDBDriverError();
        bool loadDataFromDB(std::vector<ImageInfo*>* data);
        static void read(const cv::FileNode& node, ImageInfo& x, const ImageInfo& default_value = ImageInfo());
        bool load_from_db = false;
        std::string userID;
        DBDriver* driver;
};
};

#endif // SERIALIZEDPATTERNDATAINPUT_HPP

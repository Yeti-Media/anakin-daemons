#ifndef SERIALIZEDPATTERNDATAINPUT_HPP
#define SERIALIZEDPATTERNDATAINPUT_HPP

#include <ImageInfo.hpp>

namespace Anakin {

class SerializedPatternDataInput {
    public:
        SerializedPatternDataInput(std::string inputFolder, bool useCache=false);
        bool nextInput(ImageInfo** output);
        void reload();
    protected:
    private:
        std::string inputFolder;
        std::vector<ImageInfo*>* cache;
        bool useCache;
        bool loaded;
        uint current;
        void loadData(std::vector<ImageInfo*>* data);
        static void read(const cv::FileNode& node, ImageInfo& x, const ImageInfo& default_value = ImageInfo());
};
};

#endif // SERIALIZEDPATTERNDATAINPUT_HPP

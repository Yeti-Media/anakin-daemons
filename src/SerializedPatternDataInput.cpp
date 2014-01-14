#include "SerializedPatternDataInput.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;

SerializedPatternDataInput::SerializedPatternDataInput(string inputFolder, bool useCache) {
    this->inputFolder = inputFolder;
    this->cache = new vector<ImageInfo*>(0);
    this->useCache = useCache;
    this->loaded = false;
    this->current = -1;
}

bool SerializedPatternDataInput::nextInput(ImageInfo** output) {
    if (this->current < 0) {
        if (!this->useCache || !this->loaded) {
            loadData(this->cache);
            this->loaded = true;
        }
        this->current = 0;
    }
    if (this->current == this->cache->size()) {
        return false;
    } else {
        *output = this->cache->at(this->current);
        this->current++;
        return true;
    }
}

void SerializedPatternDataInput::reload() {
    this->current = -1;
}

void SerializedPatternDataInput::loadData(vector<ImageInfo*>* data) {
    data->clear();
    if(fs::exists( this->inputFolder )) {
        fs::directory_iterator end_itr; // default construction yields past-the-end
        for (fs::directory_iterator itr( this->inputFolder ); itr != end_itr; ++itr ) {

            if (!fs::is_directory(itr->status())) {
                ImageInfo *ii = new ImageInfo();
                cv::FileStorage fstorage(itr->path().c_str(), cv::FileStorage::READ);
                cv::FileNode n = fstorage.root();//["ImageInfo"];
                read(n, *ii);
                fstorage.release();
                data->push_back(ii);
            }

        }

    } else {
        std::cout << "SerializedPatternDataInput#loadData : directory : " << this->inputFolder << " doesn't exist\n";
        exit(-1);
    }
}

void SerializedPatternDataInput::read(const cv::FileNode& node, ImageInfo& x, const ImageInfo& default_value) {
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}

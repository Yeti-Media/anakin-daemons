#include "processing/SerializedPatternDataInput.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;

SerializedPatternDataInput::SerializedPatternDataInput(std::string userID) {
    if (!initAndConnectDriver()) {
        reportDBDriverError();
        exit(-1);
    }
    this->userID = userID;
    this->cache = new vector<ImageInfo*>(0);
    this->loaded = false;
    this->current = -1;
}

bool SerializedPatternDataInput::nextInput(ImageInfo** output) {
    if (this->current < 0) {
        if (!this->loaded) {
            this->cache->clear();
            if (!loadDataFromDB(this->cache)) {
                reportDBDriverError();
                exit(-1);
            }
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

//PRIVATE

void SerializedPatternDataInput::loadData(vector<ImageInfo*>* data, std::string rawData) {
    std::string xmlData = "<?xml version=\"1.0\"?>";
    xmlData.append(rawData);
    ImageInfo *ii = new ImageInfo();
    cv::FileStorage fstorage(xmlData.c_str(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
    cv::FileNode n = fstorage.root();
    read(n, *ii);
    fstorage.release();
    data->push_back(ii);
}

void SerializedPatternDataInput::read(const cv::FileNode& node, ImageInfo& x, const ImageInfo& default_value) {
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}

bool SerializedPatternDataInput::initAndConnectDriver() {
    this->driver = new DBDriver();
    return this->driver->connect();
}

void SerializedPatternDataInput::reportDBDriverError() {
    std::cerr << this->driver->lastMessageReceived << std::endl;
}

bool SerializedPatternDataInput::loadDataFromDB(std::vector<ImageInfo*>* data) {
    bool error=false;
    int uid = std::stoi(this->userID);
    std::vector<int> userPatterns = this->driver->getUserPatterns(uid, &error);
    if (error) {
        return false;
    }
    for (uint up = 0; up < userPatterns.size(); up++) {
        int pid = userPatterns.at(up);
        DBPattern* dbp;
        bool patternError = false;
        if (!this->driver->retrievePattern(pid,&patternError, true, &dbp)) {
            return false;
        }
        loadData(data, dbp->getData());
    }
    return true;
}

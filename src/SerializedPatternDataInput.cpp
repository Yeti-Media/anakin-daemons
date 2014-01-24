#include "SerializedPatternDataInput.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;

SerializedPatternDataInput::SerializedPatternDataInput(string inputFolder, bool useInputFolderAsUserID, bool useCache) {
    if (useInputFolderAsUserID)  {
        this->load_from_db = true;
        this->userID = inputFolder;
        if (!initAndConnectDriver()) {
            reportDBDriverError();
            exit(-1);
        }
    } else {
        this->load_from_db = false;
        this->inputFolder = inputFolder;
    }

    this->cache = new vector<ImageInfo*>(0);
    this->useCache = useCache;
    this->loaded = false;
    this->current = -1;
}

bool SerializedPatternDataInput::nextInput(ImageInfo** output) {
    if (this->current < 0) {
        if (!this->useCache || !this->loaded) {
            if (this->load_from_db) {
                this->cache->clear();
                if (!loadDataFromDB(this->cache)) {
                    reportDBDriverError();
                    exit(-1);
                }
            } else {
                loadData(this->cache);
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

void SerializedPatternDataInput::loadData(vector<ImageInfo*>* data, bool loadFromMemory) {
    if (!loadFromMemory) {
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
    } else {
        //use inputFolder as data input
        std::string xmlData = "<?xml version=\"1.0\"?>";
        xmlData.append(this->inputFolder);
        ImageInfo *ii = new ImageInfo();
        cv::FileStorage fstorage(xmlData.c_str(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::FileNode n = fstorage.root();//["ImageInfo"];
        read(n, *ii);
        fstorage.release();
        data->push_back(ii);
    }
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
    std::vector<int> userPatterns = this->driver->getUserPatterns(this->userID, &error);
    if (error) {
        return false;
    }
    for (uint up = 0; up < userPatterns.size(); up++) {
        int pid = userPatterns.at(up);
        DBPattern* dbp;
        if (!this->driver->retrievePattern(pid, true, &dbp)) {
            return false;
        }
        this->inputFolder = dbp->getData();
        loadData(data, true);
    }
    return true;
}

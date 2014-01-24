#include "HistogramsIO.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;
#include "ResultWriter.hpp"

using namespace Anakin;
using namespace std;
using namespace cv;

HistogramsIO::HistogramsIO(string baseFolder, bool useFolderAsUserID) {
    if (useFolderAsUserID) {
        this->db_user_id = baseFolder;
        this->load_from_db = true;
        if (!initAndConnectDriver()) {
            reportDBDriverError();
            exit(-1);
        }
    } else {
        this->baseFolder = baseFolder;
        this->load_from_db = false;
    }
    this->colorHistograms = new vector<Histogram*>(0);
    this->grayHistograms = new vector<Histogram*>(0);
    this->hsvHistograms = new vector<Histogram*>(0);
}

void HistogramsIO::load(const char mode) {
    if (this->load_from_db) {
        this->colorHistograms->clear();
        this->grayHistograms->clear();
        this->hsvHistograms->clear();
        if (!loadFromDB(mode & LANDSCAPE)) {
            reportDBDriverError();
            exit(-1);
        }
    } else {
        string subfolder = mode & LANDSCAPE? "landscape/" : "pattern/";
        if (mode & COLOR) {
            load(this->baseFolder+subfolder+"color/", this->colorHistograms, true);
        }
        if (mode & GRAY) {
            load(this->baseFolder+subfolder+"gray/", this->grayHistograms, true);
        }
        if (mode & HSV) {
            load(this->baseFolder+subfolder+"hsv/", this->hsvHistograms, true);
        }
    }
}

void HistogramsIO::save(std::vector<Histogram*>* input, const char mode, bool saveToFile) {
    string subfolder = mode & LANDSCAPE? "landscape/" : "pattern/";
    string subSubFolder = mode & COLOR? "color/" : ((mode & GRAY)? "gray/" : "hsv/");
    string extension = mode & YAML ? ".yml" : ".xml";
    for (uint i = 0; i < input->size(); i++) {
        Histogram* current = input->at(i);
        string filename = saveToFile? (this->baseFolder+subfolder+subSubFolder+current->getLabel()+extension) : ("output" + extension);
        save(filename, current, saveToFile);
    }
}

vector<Histogram*>* HistogramsIO::getColorHistograms() {
    return this->colorHistograms;
}

vector<Histogram*>* HistogramsIO::getGrayHistograms() {
    return this->grayHistograms;
}

vector<Histogram*>* HistogramsIO::getHSVHistograms() {
    return this->hsvHistograms;
}

//PRIVATE

void HistogramsIO::load(string baseFolder, vector<Histogram*>* output, bool loadFromFile) {
    loadData(output, baseFolder, loadFromFile);
}

void HistogramsIO::save(string filename, Histogram* histogram, bool saveToFile) {
    FileStorage fs(filename, saveToFile? FileStorage::WRITE : (FileStorage::WRITE|FileStorage::MEMORY));
    write(fs, "Histogram", *histogram);
    if (saveToFile) {
        fs.release();
    } else {
        string data = fs.releaseAndGetString();
        bool isLandscape = histogram->isMinMax() || histogram->hasAvg();
        bool color = histogram->getChannels() == 3;
        bool hsv = histogram->getChannels() == 2;
        wcout << ResultWriter::output(isLandscape? ResultWriter::RW_LANDSCAPE : ResultWriter::RW_HISTOGRAMS, data, color? ResultWriter::RW_COLOR : (hsv? ResultWriter::RW_HSV : ResultWriter::RW_GRAY)) << endl;
    }
}

void HistogramsIO::read(const cv::FileNode& node, Histogram& x, const Histogram& default_value) {
    if(node.empty())
        x = default_value;
    else
        x.read(node);
}

void HistogramsIO::write(FileStorage& fs, const std::string&, const Histogram& x) {
    x.write(fs);
}

void HistogramsIO::loadData(vector<Histogram*>* data, string folder, bool loadFromFile) {
    if (loadFromFile) {
        data->clear();
        if(fs::exists( folder )) {
            fs::directory_iterator end_itr; // default construction yields past-the-end
            for (fs::directory_iterator itr( folder ); itr != end_itr; ++itr ) {

                if (!fs::is_directory(itr->status())) {
                    Histogram *ii = new Histogram();
                    cv::FileStorage fstorage(itr->path().c_str(), cv::FileStorage::READ);
                    cv::FileNode n = fstorage.root();
                    read(n, *ii);
                    fstorage.release();
                    data->push_back(ii);
                }

            }
        } else {
            std::cout << "HistogramsIO#loadData : directory : " << folder << " doesn't exist\n";
            exit(-1);
        }
    } else {
        //folder is used as input string
        std::string xmlData = "<?xml version=\"1.0\"?>";
        xmlData.append(folder);
        Histogram *ii = new Histogram();
        cv::FileStorage fstorage(xmlData.c_str(), cv::FileStorage::READ | cv::FileStorage::MEMORY);
        cv::FileNode n = fstorage.root();
        read(n, *ii);
        fstorage.release();
        data->push_back(ii);
    }

}

bool HistogramsIO::loadFromDB(bool loadLandscapes) {
    bool error=false;
    std::vector<int> userHorls = loadLandscapes?this->driver->getUserLandscapes(this->db_user_id, &error) : this->driver->getUserHistograms(this->db_user_id, &error);
    if (error) {
        return false;
    }
    for (uint uh = 0; uh < userHorls.size(); uh++) {
        int pid = userHorls.at(uh);
        DBHistogram* dbh;
        if (loadLandscapes) {
            if (!this->driver->retrieveLandscape(pid, true, &dbh)) {
                return false;
            }
        } else {
            if (!this->driver->retrieveHistogram(pid, true, &dbh)) {
                return false;
            }
        }
        std::string cdata = dbh->getColorData();
        std::string gdata = dbh->getGrayData();
        std::string hdata = dbh->getHSVData();
        loadData(this->colorHistograms, cdata, false);
        loadData(this->grayHistograms, gdata, false);
        loadData(this->hsvHistograms, hdata, false);
    }
    return true;
}

bool HistogramsIO::initAndConnectDriver() {
    this->driver = new DBDriver();
    return this->driver->connect();
}

void HistogramsIO::reportDBDriverError() {
    std::cerr << this->driver->lastMessageReceived << std::endl;
}

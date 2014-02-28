#include "XMLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Constants.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace Anakin;

XMLoader::XMLoader(std::string path) {
    this->path = path;
    unsigned lastSeparator = path.find_last_of("/\\");
    this->inputAsFolder = lastSeparator != std::string::npos;
}

std::vector<DBPattern*>* XMLoader::loadAsPattern() {
    std::vector<std::string>* files = getFilePaths();
    std::vector<DBPattern*>* patterns = new std::vector<DBPattern*>(0);
    for (uint f = 0; f < files->size(); f++) {
        std::string filepath = files->at(f);
        std::string label = getFilename(filepath);
        std::string data = loadFile(filepath);
        DBPattern* pattern = new DBPattern(label, data);
        patterns->push_back(pattern);
    }
    return patterns;
}

std::vector<DBHistogram*>* XMLoader::loadAsHistogram() {
    return loadAsHORL(false);
}

std::vector<DBHistogram*>* XMLoader::loadAsLandscape() {
    return loadAsHORL(true);
}

std::string XMLoader::loadFile(const std::string filename) {
    std::string data;
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    data = buffer.str();
    return data;
}

std::string XMLoader::getFilename (const std::string& str) {
    unsigned lastSeparator = str.find_last_of("/\\");
    std::string file = lastSeparator == std::string::npos ? str : str.substr(lastSeparator+1);
    unsigned lastDot = file.find_last_of(".");
    std::string name = lastDot == std::string::npos ? file : file.substr(0, lastDot);
    return name;
}

//PRIVATE

std::vector<DBHistogram*>* XMLoader::loadAsHORL(bool isLandscape) {
    if (!this->inputAsFolder) {
        std::cerr << "path must lead to a folder to load landscapes or histograms" << std::endl;
        exit(-1);
    }
    char mode = isLandscape? Constants::LANDSCAPE : Constants::HISTOGRAM;
    std::vector<std::string>* cfiles = getFilePaths(mode | Constants::COLOR);
    std::vector<std::string>* gfiles = getFilePaths(mode | Constants::GRAY);
    std::vector<std::string>* hfiles = getFilePaths(mode | Constants::HSV);
    std::vector<DBHistogram*>* horls = new std::vector<DBHistogram*>(0);
    for (uint f = 0; f < cfiles->size(); f++) {
        std::string cfilepath = cfiles->at(f);
        std::string gfilepath = gfiles->at(f);
        std::string hfilepath = hfiles->at(f);
        std::string label = getFilename(cfilepath);
        std::string cdata = loadFile(cfilepath);
        std::string gdata = loadFile(gfilepath);
        std::string hdata = loadFile(hfilepath);
        DBHistogram* horl = new DBHistogram(label, isLandscape);
        horl->setColorData(cdata);
        horl->setGrayData(gdata);
        horl->setHSVData(hdata);
        horls->push_back(horl);
    }
    return horls;
}

std::vector<std::string>* XMLoader::getFilePaths(char mode, bool reload) {
    static std::vector<std::string>* ppaths = new std::vector<std::string>(0);
    static std::vector<std::string>* cpaths = new std::vector<std::string>(0);
    static std::vector<std::string>* gpaths = new std::vector<std::string>(0);
    static std::vector<std::string>* hpaths = new std::vector<std::string>(0);
    static char loaded = 0;
    //std::cout << "loaded : " << (loaded + 0) << std::endl;
    //std::cout << "mode : " << (mode + 0) << std::endl;
    if (loaded == 0 || (loaded & mode) < loaded || reload) {
        if (mode & Constants::COLOR) {
            cpaths->clear();
        } else if (mode & Constants::GRAY) {
            gpaths->clear();
        } else if (mode & Constants::HSV) {
            hpaths->clear();
        } else {
            ppaths->clear();
        }
        std::string filepath = this->path;
        if (this->inputAsFolder) {
            if (mode & Constants::LANDSCAPE) {
                filepath = filepath.append("landscape/");
            } else if (mode & Constants::HISTOGRAM) {
                filepath = filepath.append("pattern/");
            }
            if (mode & Constants::COLOR) {
                filepath = filepath.append("color/");
            } else if (mode & Constants::GRAY) {
                filepath = filepath.append("gray/");
            } else if (mode & Constants::HSV) {
                filepath = filepath.append("hsv/");
            }
            if(fs::exists( filepath )) {

                fs::directory_iterator end_itr; // default construction yields past-the-end
                for (fs::directory_iterator itr( filepath ); itr != end_itr; ++itr ) {

                    if (!fs::is_directory(itr->status()) && hasEnding(itr->path().string(), ".xml")) {
                        if (mode & Constants::COLOR) {
                            cpaths->push_back(itr->path().string());
                        } else if (mode & Constants::GRAY) {
                            gpaths->push_back(itr->path().string());
                        } else if (mode & Constants::HSV) {
                            hpaths->push_back(itr->path().string());
                        } else {
                            ppaths->push_back(itr->path().string());
                        }
                    }

                }
            } else {
                std::cout << "directory : " << filepath << " doesn't exist\n";
                exit(-1);
            }
        } else {
            ppaths->push_back(filepath);
        }
        loaded = loaded | mode;
    }
    if (mode & Constants::COLOR) {
        //std::cout << "cpaths->size() : " << cpaths->size() << std::endl;
        return cpaths;
    } else if (mode & Constants::GRAY) {
        //std::cout << "gpaths->size() : " << gpaths->size() << std::endl;
        return gpaths;
    } else if (mode & Constants::HSV) {
        //std::cout << "hpaths->size() : " << hpaths->size() << std::endl;
        return hpaths;
    } else {
        return ppaths;
    }
}

bool XMLoader::hasEnding(std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
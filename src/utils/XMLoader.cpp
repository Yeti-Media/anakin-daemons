#include "utils/XMLoader.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/Constants.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;
#include <algorithm>

using namespace Anakin;
using namespace std;

XMLoader::XMLoader(string path) {
	this->path = path;
	unsigned lastSeparator = path.find_last_of("/\\");
	this->inputAsFolder = lastSeparator != string::npos
			&& lastSeparator == (path.size() - 1);
}

vector<DBPattern*>* XMLoader::loadAsPattern() {
	vector<string>* files = getFilePaths();
	vector<DBPattern*>* patterns = new vector<DBPattern*>(0);
	for (uint f = 0; f < files->size(); f++) {
		string filepath = files->at(f);
		string data = loadFile(filepath);
		DBPattern* pattern = new DBPattern(data);
		patterns->push_back(pattern);
	}
	delete files;
	return patterns;
}

vector<DBHistogram*>* XMLoader::loadAsHistogram() {
	return loadAsHORL(false);
}

vector<DBHistogram*>* XMLoader::loadAsLandscape() {
	return loadAsHORL(true);
}

ImageInfo* XMLoader::dbpatternToImageInfo(DBPattern* dbp) {
	string xmlData = "";
	xmlData.append(dbp->getData());
	ImageInfo *ii = new ImageInfo();
	string label = to_string(dbp->getID());
	cv::FileStorage fstorage(xmlData.c_str(),
			cv::FileStorage::READ | cv::FileStorage::MEMORY);
	cv::FileNode n = fstorage.root();
	ii->read(n);
	fstorage.release();
	ii->setLabel(label);
	return ii;
}

string XMLoader::loadFile(const string filename) {
	string data;
	ifstream t(filename);
	stringstream buffer;
	buffer << t.rdbuf();
	data = buffer.str();
	return data;
}

//PRIVATE

vector<DBHistogram*>* XMLoader::loadAsHORL(bool isLandscape) {
	if (!this->inputAsFolder) {
		cerr << "path must lead to a folder to load landscapes or histograms"
				<< endl;
		exit(EXIT_FAILURE);
	}
	char mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
	vector<string>* cfiles = getFilePaths(mode | Constants::COLOR);
	vector<string>* gfiles = getFilePaths(mode | Constants::GRAY);
	vector<string>* hfiles = getFilePaths(mode | Constants::HSV);
	vector<DBHistogram*>* horls = new vector<DBHistogram*>(0);
	for (uint f = 0; f < cfiles->size(); f++) {
		string cfilepath = cfiles->at(f);
		string gfilepath = gfiles->at(f);
		string hfilepath = hfiles->at(f);
		string cdata = loadFile(cfilepath);
		string gdata = loadFile(gfilepath);
		string hdata = loadFile(hfilepath);
		DBHistogram* horl = new DBHistogram(isLandscape);
		horl->setColorData(cdata);
		horl->setGrayData(gdata);
		horl->setHSVData(hdata);
		horls->push_back(horl);
	}
	return horls;
}

vector<string>* XMLoader::getFilePaths(char mode, bool reload) {
	static vector<string>* ppaths = new vector<string>(0);
	static vector<string>* cpaths = new vector<string>(0);
	static vector<string>* gpaths = new vector<string>(0);
	static vector<string>* hpaths = new vector<string>(0);
	static char loaded = 0;
	//cout << "loaded : " << (loaded + 0) << endl;
	//cout << "mode : " << (mode + 0) << endl;
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
		string filepath = this->path;
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
			if (fs::exists(filepath)) {
				vector<fs::path> filePaths;
				copy(fs::directory_iterator(filepath), fs::directory_iterator(),
						back_inserter(filePaths));
				sort(filePaths.begin(), filePaths.end());
				vector<fs::path>::const_iterator fileItr(filePaths.begin());
				vector<fs::path>::const_iterator endItr(filePaths.end());

				while (fileItr != endItr) {
					if (!fs::is_directory(*fileItr)
							&& hasEnding((*fileItr).string(), ".xml")) {
						if (mode & Constants::COLOR) {
							cpaths->push_back((*fileItr).string());
						} else if (mode & Constants::GRAY) {
							gpaths->push_back((*fileItr).string());
						} else if (mode & Constants::HSV) {
							hpaths->push_back((*fileItr).string());
						} else {
							ppaths->push_back((*fileItr).string());
						}
					}
					fileItr++;
				}
			} else {
				cout << "directory : " << filepath << " doesn't exist\n";
				exit(-1);
			}
		} else {
			ppaths->push_back(filepath);
		}
		loaded = loaded | mode;
	}
	if (mode & Constants::COLOR) {
		//cout << "cpaths->size() : " << cpaths->size() << endl;
		return cpaths;
	} else if (mode & Constants::GRAY) {
		//cout << "gpaths->size() : " << gpaths->size() << endl;
		return gpaths;
	} else if (mode & Constants::HSV) {
		//cout << "hpaths->size() : " << hpaths->size() << endl;
		return hpaths;
	} else {
		return ppaths;
	}
}

bool XMLoader::hasEnding(string const &fullString, string const &ending) {
	if (fullString.length() >= ending.length()) {
		return (0
				== fullString.compare(fullString.length() - ending.length(),
						ending.length(), ending));
	} else {
		return false;
	}
}

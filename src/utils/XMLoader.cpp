#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <opencv2/core/core.hpp>
#include <sys/types.h>
#include <utils/Constants.hpp>
#include <utils/files/Files.hpp>
#include <utils/XMLoader.hpp>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <algorithm>

namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;
using namespace cv;

XMLoader::XMLoader(const string & path) {
	this->path = path;
	unsigned lastSeparator = path.find_last_of("/\\");
	this->inputAsFolder = lastSeparator != string::npos
			&& lastSeparator == (path.size() - 1);
}

Ptr<vector<Ptr<DBPattern>>> XMLoader::loadAsPattern(bool filePatterns) {
	Ptr<vector<string>> files = getFilePaths();
	Ptr<vector<Ptr<DBPattern>>> patterns = makePtr<vector<Ptr<DBPattern>>>();
	for (uint f = 0; f < files->size(); f++) {
		string filepath = files->at(f);
		Ptr<DBPattern> pattern;
		if (filePatterns) {
			pattern = makePtr<DBPattern>(true, makePtr<string>(filepath));
		} else {
			pattern = makePtr<DBPattern>(false, get_file_contents(filepath));
		}
		patterns->push_back(pattern);
	}
	//FIXME do not delete this static var. REFACTOR!
	return patterns;
}

Ptr<vector<Ptr<DBHistogram>>> XMLoader::loadAsHistogram(bool filePatterns) {
	return loadAsHORL(false, filePatterns);
}

Ptr<vector<Ptr<DBHistogram>>> XMLoader::loadAsLandscape(bool filePatterns) {
	return loadAsHORL(true, filePatterns);
}

Ptr<ImageInfo> XMLoader::dbpatternToImageInfo(const Ptr<DBPattern> & dbp) {
	string xmlData = "";
	xmlData.append(*dbp->getData());
	Ptr<ImageInfo> ii = makePtr<ImageInfo>();
	string label = to_string(dbp->getID());
	cv::FileStorage fstorage(xmlData.c_str(),
			cv::FileStorage::READ | cv::FileStorage::MEMORY);
	cv::FileNode n = fstorage.root();
	ii->read(n);
	fstorage.release();
	ii->setLabel(label);
	return ii;
}

//PRIVATE

Ptr<vector<Ptr<DBHistogram>>> XMLoader::loadAsHORL(bool isLandscape,
		bool filePatterns) {
	if (!this->inputAsFolder) {
		cerr << "path must lead to a folder to load landscapes or histograms"
		<< endl;
		exit(EXIT_FAILURE);
	}
	char mode = isLandscape ? Constants::LANDSCAPE : Constants::HISTOGRAM;
	Ptr<vector<string>> cfiles = getFilePaths(mode | Constants::COLOR);
	Ptr<vector<string>> gfiles = getFilePaths(mode | Constants::GRAY);
	Ptr<vector<string>> hfiles = getFilePaths(mode | Constants::HSV);
	Ptr<vector<Ptr<DBHistogram>>> horls = makePtr<vector<Ptr<DBHistogram>>>(0);
	for (uint f = 0; f < cfiles->size(); f++) {
		string cfilepath = cfiles->at(f);
		string gfilepath = gfiles->at(f);
		string hfilepath = hfiles->at(f);
		Ptr<DBHistogram> horl;
		if (filePatterns) {
			horl = makePtr<DBHistogram>(true, isLandscape);
			horl->setColorData(cfilepath);
			horl->setGrayData(gfilepath);
			horl->setHSVData(hfilepath);
		} else {
			horl = makePtr<DBHistogram>(false, isLandscape);
			horl->setColorData(*get_file_contents(cfilepath));
			horl->setGrayData(*get_file_contents(gfilepath));
			horl->setHSVData(*get_file_contents(hfilepath));
		}
		horls->push_back(horl);
	}
	return horls;
}

Ptr<vector<string>> XMLoader::getFilePaths(char mode, bool reload) {
	//FIXME MEMORY LEAKS
	static Ptr<vector<string>> ppaths = makePtr<vector<string>>();
	static Ptr<vector<string>> cpaths = makePtr<vector<string>>();
	static Ptr<vector<string>> gpaths = makePtr<vector<string>>();
	static Ptr<vector<string>> hpaths = makePtr<vector<string>>();
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

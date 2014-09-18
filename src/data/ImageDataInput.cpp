#include "data/ImageDataInput.hpp"
#include <opencv2/imgcodecs.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;
#include <algorithm>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>

using namespace Anakin;
using namespace std;
using namespace cv;

ImageDataInput::ImageDataInput(const string & imagesFolder, bool loadOnDemand) {
	this->imagesFolder = imagesFolder;
	this->loadOnDemand = loadOnDemand;
	if (this->loadOnDemand) {
		initializeIterator();
	} else {
		loadImages();
	}
}

ImageDataInput::~ImageDataInput() {
}

bool ImageDataInput::nextInput(Ptr<Anakin::Img> & output) {
	static int loadedFiles = 0;
	Ptr<Mat> nextMat;
	Ptr<string> label;
	if (this->loadOnDemand) {
//        if (loadedFiles > 0 && (loadedFiles % 1000 == 0)) {
//            cout << "\n1k files loaded, total loaded files : " << loadedFiles << endl;
//        }
		if (nextFile(nextMat, label)) {
			output = makePtr<Img>(nextMat, *label.get());
			loadedFiles += 1;
			return true;
		}
	} else {
		if (!this->images.empty()) {
			nextMat = this->images.back();
			label = this->labels.back();
			this->images.pop_back();
			this->labels.pop_back();
			output = makePtr<Img>(nextMat, *label.get());
			return true;
		}
	}
	return false;
}

void ImageDataInput::reload() {
	loadImages();
}

int ImageDataInput::imagesToLoad() {
	static int files = scanFiles();
	return files;
}

//PROTECTED

void ImageDataInput::loadImages() {
	int loadedFiles = 0;
	if (fs::exists(imagesFolder)) {
		int files = imagesToLoad();
		this->images.resize(files);
		this->labels.resize(files);
		int idx = 0;
		fs::directory_iterator end_itr; // default construction yields past-the-end
		for (fs::directory_iterator itr(imagesFolder); itr != end_itr; ++itr) {
//            if (loadedFiles > 0 && (loadedFiles % 1000 == 0)) {
//                cout << "1k files loaded, total loaded files : " << loadedFiles << endl;
//            }
			if (!fs::is_directory(itr->status())) {
				//cout << "Loading image : " << itr->path().c_str() << "\n";
				Ptr<Mat> img = makePtr<Mat>(imread(itr->path().c_str()));
				if (!img->data) {
					cerr << "Error loading image : " << itr->path().c_str()
							<< endl;
					LOG_F("ERROR")<< "Error loading image : " << itr->path().c_str();
					exit(EXIT_FAILURE);
				}
				this->images.at(idx) = img;
				Ptr<string> label = makePtr<string>(itr->path().string());
				this->labels.at(idx) = label;
				loadedFiles++;
				idx++;
			}

		}

	} else {
		cerr << "directory : " << imagesFolder << " doesn't exist" << endl;
		LOG_F("ERROR")<< "directory : " << imagesFolder << " doesn't exist";
		exit(EXIT_FAILURE);
	}
	//cout << "total loaded files : " << loadedFiles << endl;
}

void ImageDataInput::initializeIterator() {
	if (fs::exists(imagesFolder)) {
		orderFiles();
	} else {
		cerr << "directory : " << imagesFolder << " doesn't exist" << endl;
		LOG_F("ERROR")<< "directory : " << imagesFolder << " doesn't exist";
		exit(EXIT_FAILURE);
	}
}

bool ImageDataInput::nextFile(Ptr<Mat> & imat, Ptr<string> & label) {
	vector<fs::path>::const_iterator di = *this->fileItr;
	vector<fs::path>::const_iterator end_itr(filePaths.end());
	if (di != end_itr) {
		if (!fs::is_directory(*di)) {
			//cout << "reading " << *di << endl;
			Ptr<Mat> img = makePtr<Mat>(imread((*di).string().c_str()));
			if (!img->data) {
				cerr << "Error loading image : " << *di << endl;
				LOG_F("ERROR")<< "Error loading image : " << *di;
				exit(EXIT_FAILURE);
			}
			imat = img;
			label = makePtr<string>((*di).string());
			(*this->fileItr)++;
		} else {
			//cout << "directory found" << endl;
			(*this->fileItr)++;
			return nextFile(imat, label);
		}
		return true;
	} else {
		return false;
	}
}

//PRIVATE

int ImageDataInput::scanFiles() {
	int files = 0;
	fs::directory_iterator end_itr; // default construction yields past-the-end
	for (fs::directory_iterator itr(imagesFolder); itr != end_itr; ++itr) {
		if (!fs::is_directory(itr->status())) {
			files++;
		}
	}
	return files;
}

void ImageDataInput::orderFiles() {
	copy(fs::directory_iterator(imagesFolder), fs::directory_iterator(),
			back_inserter(filePaths));
	sort(filePaths.begin(), filePaths.end());
	fileItr = makePtr<vector<fs::path>::const_iterator>(filePaths.begin());
}

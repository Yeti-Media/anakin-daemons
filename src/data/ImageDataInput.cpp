#include "data/ImageDataInput.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;
#include <algorithm>

using namespace Anakin;

ImageDataInput::ImageDataInput(std::string imagesFolder, bool loadOnDemand) {
	this->imagesFolder = imagesFolder;
	this->loadOnDemand = loadOnDemand;
	if (this->loadOnDemand) {
		initializeIterator();
	} else {
		this->images = new std::vector<cv::Mat>(0);
		this->labels = new std::vector<std::string>(0);
		loadImages(this->images);
	}
}

bool ImageDataInput::nextInput(Anakin::Img** output) {
	static int loadedFiles = 0;
	cv::Mat nextMat;
	std::string label;
	if (this->loadOnDemand) {
//        if (loadedFiles > 0 && (loadedFiles % 1000 == 0)) {
//            std::cout << "\n1k files loaded, total loaded files : " << loadedFiles << std::endl;
//        }
		if (nextFile(&nextMat, &label)) {
			Img* nextImg = new Img(nextMat, label);
			*output = nextImg;
			loadedFiles += 1;
			return true;
		}
	} else {
		if (!this->images->empty()) {
			nextMat = this->images->back();
			label = this->labels->back();
			this->images->pop_back();
			this->labels->pop_back();
			Img* nextImg = new Img(nextMat, label);
			*output = nextImg;
			return true;
		}
	}
	return false;
}

void ImageDataInput::reload() {
	loadImages(this->images);
}

int ImageDataInput::imagesToLoad() {
	static int files = scanFiles();
	return files;
}

//PROTECTED

void ImageDataInput::loadImages(std::vector<cv::Mat>* images) {
	int loadedFiles = 0;
	if (fs::exists(imagesFolder)) {
		int files = imagesToLoad();
		this->images->resize(files);
		this->labels->resize(files);
		int idx = 0;
		fs::directory_iterator end_itr; // default construction yields past-the-end
		for (fs::directory_iterator itr(imagesFolder); itr != end_itr; ++itr) {
//            if (loadedFiles > 0 && (loadedFiles % 1000 == 0)) {
//                std::cout << "1k files loaded, total loaded files : " << loadedFiles << std::endl;
//            }
			if (!fs::is_directory(itr->status())) {
				//std::cout << "Loading image : " << itr->path().c_str() << "\n";
				cv::Mat img = cv::imread(itr->path().c_str());
				if (!img.data) {
					std::cerr << "Error loading image : " << itr->path().c_str() << "\n";
					exit(-1);
				}
				this->images->at(idx) = img;
				std::string label(itr->path().string());
				this->labels->at(idx) = label;
				loadedFiles++;
				idx++;
			}

		}

	} else {
		std::cerr << "directory : " << imagesFolder << " doesn't exist\n";
		exit(-1);
	}
	//std::cout << "total loaded files : " << loadedFiles << std::endl;
}

void ImageDataInput::initializeIterator() {
	if (fs::exists(imagesFolder)) {
		orderFiles();
	} else {
		std::cerr << "directory : " << imagesFolder << " doesn't exist\n";
		exit(-1);
	}
}

bool ImageDataInput::nextFile(cv::Mat* imat, std::string * label) {
	std::vector<fs::path>::const_iterator di = *this->fileItr;
	std::vector<fs::path>::const_iterator end_itr(filePaths.end());
	if (di != end_itr) {
		if (!fs::is_directory(*di)) {
			//std::cout << "reading " << *di << std::endl;
			cv::Mat img = cv::imread((*di).string().c_str());
			if (!img.data) {
				std::cerr << "Error loading image : " << *di << "\n";
				exit(-1);
			}
			*imat = img;
			*label = (*di).string();
			(*this->fileItr)++;
		} else {
			//std::cout << "directory found" << std::endl;
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
	std::copy(fs::directory_iterator(imagesFolder), fs::directory_iterator(),
			std::back_inserter(filePaths));
	std::sort(filePaths.begin(), filePaths.end());
	fileItr = new std::vector<fs::path>::const_iterator(filePaths.begin());
}

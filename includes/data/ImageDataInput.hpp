#ifndef IMAGEDATAINPUT_H
#define IMAGEDATAINPUT_H

#include <data/DataInput.hpp>
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace std;
namespace Anakin {

class ImageDataInput: public DataInput {
public:
	ImageDataInput(string imagesFolder, bool loadOnDemand = false);
	~ImageDataInput();
	bool nextInput(Anakin::Img** output);
	void reload();
	int imagesToLoad();
protected:
	void loadImages();
	void initializeIterator();
	bool nextFile(cv::Mat* imat, string * label);
private:
	int scanFiles();
	string imagesFolder;
	vector<cv::Mat> images;
	vector<string> labels;
	//fs::directory_iterator* itr;
	//fs::directory_iterator end_itr;
	bool loadOnDemand = false;
	vector<fs::path> filePaths;
	vector<fs::path>::const_iterator* fileItr;
	void orderFiles();
};

}
;

#endif // IMAGEDATAINPUT_H

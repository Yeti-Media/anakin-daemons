#ifndef IMAGEDATAINPUT_H
#define IMAGEDATAINPUT_H

#include <data/DataInput.hpp>
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
namespace fs = boost::filesystem;

using namespace std;
using namespace cv;

namespace Anakin {

class ImageDataInput: public DataInput {
public:
	ImageDataInput(const string & imagesFolder, bool loadOnDemand = false);
	~ImageDataInput();
	bool nextInput(Ptr<Anakin::Img> & output);
	void reload();
	int imagesToLoad();
protected:
	void loadImages();
	void initializeIterator();
	bool nextFile(Ptr<Mat> & imat, Ptr<string> & label);
private:
	int scanFiles();
	string imagesFolder;
	vector<Ptr<Mat>> images;
	vector<Ptr<string>> labels;
	bool loadOnDemand = false;
	vector<fs::path> filePaths;
	Ptr<vector<fs::path>::const_iterator> fileItr;
	void orderFiles();
};

}
;

#endif // IMAGEDATAINPUT_H

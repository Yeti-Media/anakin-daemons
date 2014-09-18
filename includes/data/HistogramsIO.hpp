#ifndef HISTOGRAMSIO_HPP
#define HISTOGRAMSIO_HPP

#include <data/Histogram.hpp>
#include <opencv2/core/core.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class HistogramsIO {
public:
	HistogramsIO(const string & baseFolder);
	~HistogramsIO();
	void load(const char mode);
	void save(const Ptr<vector<Ptr<Histogram>>>&input, const char mode,
	bool saveToFile = true);
	const static char COLOR = 1;
	const static char GRAY = 2;
	const static char HSV = 4;
	const static char LANDSCAPE = 8;
	const static char YAML = 16;
	const static char XML = 32;
	Ptr<vector<Ptr<Histogram>>> getColorHistograms();
	Ptr<vector<Ptr<Histogram>>> getGrayHistograms();
	Ptr<vector<Ptr<Histogram>>> getHSVHistograms();
protected:
private:
	void load(const string & baseFolder, Ptr<vector<Ptr<Histogram>>> & output);
	void save(const string & filename,const Ptr<Histogram> histogram,
	bool saveToFile);
	static void write(FileStorage & fs, const string & t,
	const Histogram & x);
	static void read(const FileNode & node, Ptr<Histogram> & x);
	void loadData(Ptr<vector<Ptr<Histogram>>> & data, const string & folder);
	string baseFolder;
	Ptr<vector<Ptr<Histogram>>> colorHistograms;
	Ptr<vector<Ptr<Histogram>>> grayHistograms;
	Ptr<vector<Ptr<Histogram>>> hsvHistograms;
};
}
;
#endif // HISTOGRAMSIO_HPP

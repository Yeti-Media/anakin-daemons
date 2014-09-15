#ifndef HISTOGRAM_HPP
#define HISTOGRAM_HPP

#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/persistence.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

namespace Anakin {

class Histogram {
public:
	Histogram(const Mat & hist, const Ptr<vector<int>> & bins, int channels,
			string label = "", bool minMax = false, bool avg = false);
	Histogram();
	~Histogram();
	Mat getHist();
	Ptr<vector<int>> getBins();
	int getChannels();
	string getLabel();
	bool isMinMax();
	bool hasAvg();
	void write(FileStorage& fs) const; //Write serialization for this class
	void read(const FileNode& node);    //Read serialization for this class
protected:
private:
	Mat hist;
	Ptr<vector<int>> bins;
	int channels;
	string label;
	bool minMax;
	bool avg;
};

}
;

#endif // HISTOGRAM_HPP

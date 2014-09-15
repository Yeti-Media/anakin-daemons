#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP

#include <data/Histogram.hpp>
#include <data/RichImg.hpp>
#include <opencv2/core/core.hpp>
#include "data/HistogramsIO.hpp"
#include "data/Img.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace cv;
namespace Anakin {

class HistogramComparator {
public:
	HistogramComparator(std::vector<Anakin::RichImg*> patterns,
			HistogramsIO* io);
	void makeAndSaveLandscape(char mode, std::string label, bool saveToFile =
			true);
	void makeAndSaveHistograms(char mode, bool saveToFile = true);
	virtual ~HistogramComparator();

	//FIXME refactor this constants to enum
	const static char COLOR = 1;
	const static char GRAY = 2;
	const static char HSV = 4;
	const static char MINMAX = 8;
	const static char AVG = 16;
	const static char YAML = 32;
	const static char XML = 64;
protected:
private:
	void cleanupHistogramVector(vector<Histogram*>* hVector);
	void pmakeAndSaveLandscape(char mode, std::string label, bool saveToFile =
			true);
	void update_minMax(Mat & minMaxHist, const Ptr<vector<Mat>> & hists,
			std::vector<int>* bins, std::vector<int> maxValues, int channels,
			bool firstPass);
	void update_average(Mat & minMaxHist, const Ptr<vector<int>> &  bins,
			int channels, int count);
	Histogram* createColorHistogram(Anakin::Img* img);
	Histogram* createGrayHistogram(Anakin::Img* img);
	Histogram* createHSVHistogram(Anakin::Img* img);
	std::vector<Anakin::RichImg*> patterns;
	HistogramsIO* io;
};

}
;

#endif // HISTOGRAMCOMPARATOR_HPP

#ifndef HISTOGRAMCOMPARATOR_HPP
#define HISTOGRAMCOMPARATOR_HPP

#include <data/Histogram.hpp>
#include <data/RichImg.hpp>
#include <opencv2/core/core.hpp>
#include "data/HistogramsIO.hpp"
#include "data/Histogram.hpp"
#include "data/Img.hpp"
#include <string>
#include <vector>

using namespace std;
using namespace cv;
namespace Anakin {

class HistogramComparator {
public:
	HistogramComparator(const Ptr<vector<Ptr<RichImg>>> & patterns,
			const Ptr<HistogramsIO> & io);
	void makeAndSaveLandscape(char mode, string label, bool saveToFile =
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
	void pmakeAndSaveLandscape(char mode, string label, bool saveToFile =
	true);
	void update_minMax(Mat & minMaxHist, const Ptr<vector<Mat>> & hists,
	const Ptr<vector<int>> & bins, const vector<int> & maxValues, int channels,
	bool firstPass);
	void update_average(Mat & minMaxHist, const Ptr<vector<int>> & bins,
	int channels, int count);
	Ptr<Histogram> createColorHistogram(const Ptr<Img> & img);
	Ptr<Histogram> createGrayHistogram(const Ptr<Img> & img);
	Ptr<Histogram> createHSVHistogram(const Ptr<Img> & img);
	Ptr<vector<Ptr<RichImg>>> patterns;
	Ptr<HistogramsIO> io;
};

}
;

#endif // HISTOGRAMCOMPARATOR_HPP

#include "data/HistogramComparator.hpp"
#include "data/DataInput.hpp"

using namespace Anakin;
using namespace cv;
using namespace std;

HistogramComparator::HistogramComparator(const Ptr<vector<Ptr<RichImg>>> & patterns,
		const Ptr<HistogramsIO> & io) {
	this->io = io;
	this->patterns = patterns;
}

void HistogramComparator::makeAndSaveLandscape(char mode, std::string label,
		bool saveToFile) {
	char pmode = mode & 120;
	if (mode & this->COLOR) {
		pmakeAndSaveLandscape(pmode | this->COLOR, label, saveToFile);
	}
	if (mode & this->GRAY) {
		pmakeAndSaveLandscape(pmode | this->GRAY, label, saveToFile);
	}
	if (mode & this->HSV) {
		pmakeAndSaveLandscape(pmode | this->HSV, label, saveToFile);
	}
}

void HistogramComparator::makeAndSaveHistograms(char mode, bool saveToFile) {
	Ptr<vector<Ptr<Histogram>>> colorHistograms = makePtr<vector<Ptr<Histogram>>>();
	Ptr<vector<Ptr<Histogram>>> grayHistograms = makePtr<vector<Ptr<Histogram>>>();
	Ptr<vector<Ptr<Histogram>>> hsvHistograms = makePtr<vector<Ptr<Histogram>>>();
	for (uint p = 0; p < this->patterns->size(); p++) {
		Ptr<Img> pattern = this->patterns->at(p)->getImage();
		if (mode & this->COLOR) {
			Ptr<Histogram> hist = createColorHistogram(pattern).constCast<Histogram>();
			colorHistograms->push_back(hist);
		}
		if (mode & this->GRAY) {
			Ptr<Histogram> hist = createGrayHistogram(pattern).constCast<Histogram>();
			grayHistograms->push_back(hist);
		}
		if (mode & this->HSV) {
			Ptr<Histogram> hist = createHSVHistogram(pattern).constCast<Histogram>();
			hsvHistograms->push_back(hist);
		}
	}
	this->io->save(colorHistograms, COLOR, saveToFile);
	this->io->save(grayHistograms, GRAY, saveToFile);
	this->io->save(hsvHistograms, HSV, saveToFile);
}

void HistogramComparator::update_minMax(Mat & minMaxHist,
		const Ptr<vector<Mat>> & hists, const Ptr<vector<int>> & bins,
		const vector<int> & maxValues, int channels, bool firstPass) {
	for (int c = 0; c < channels; c++) {
		int currentBins = bins->at(c);
		//int currentMaxValue = maxValues[c];
		Mat currentHist = hists->at(c);
		for (int i = 0; i < currentBins; i++) {
			float minVal = minMaxHist.at<float>(i, c);
			int maxChannel = channels + c;
			int averageChannel = (channels * 2) + c;
			float maxVal = minMaxHist.at<float>(i, maxChannel);
			float currentVal = currentHist.at<float>(i);
			minMaxHist.at<float>(i, averageChannel) += currentVal;
			if (firstPass && (minVal == 0 && maxVal == 0)) {
				minMaxHist.at<float>(i, c) = currentVal;
				minMaxHist.at<float>(i, maxChannel) = currentVal;
			} else if (currentVal > maxVal) {
				minMaxHist.at<float>(i, maxChannel) = currentVal;
			} else if (currentVal < minVal) {
				minMaxHist.at<float>(i, c) = currentVal;
			}
		}
	}
}

void HistogramComparator::update_average(Mat & minMaxHist,
		const Ptr<vector<int>> & bins, int channels, int count) {
	for (int c = 0; c < channels; c++) {
		int currentBins = bins->at(c);
		for (int i = 0; i < currentBins; i++) {
			int averageChannel = (channels * 2) + c;
			minMaxHist.at<float>(i, averageChannel) /= (float) count;
		}
	}
}

Ptr<Histogram> HistogramComparator::createColorHistogram(
		const Ptr<Img> & img) {
	int histSize = 256;
	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 255 };
	const float* ranges[] = { range, range, range };

	bool uniform = true; //bool accumulate = false;

	// Use the o-th and 1-st channels
	int channels[] = { 0, 1, 2 };

	/// Histograms
	MatND hist;
	Ptr<Mat> imgMat = img->getImage();

	/// Calculate the histograms for the BGR images
	calcHist(&(*imgMat), 1, channels, Mat(), hist, 1, &histSize, ranges,
			uniform, false);
	Ptr<vector<int>> bins = makePtr<vector<int>>(1, 256);
	return makePtr<Histogram>(hist, bins, 3, img->getLabel(), false, false);
}

Ptr<Histogram> HistogramComparator::createGrayHistogram(
		const Ptr<Img> & img) {
	int histSize = 256;

	/// Set the ranges ( for greyscale) )
	float range[] = { 0, histSize - 1 };
	const float* ranges[] = { range };

	bool uniform = true; //bool accumulate = false;

	// Use the o-th and 1-st channels
	int channels[] = { 0 };

	/// Histograms
	MatND hist;
	Ptr<Mat> imgMat = img->getGrayImg();

	/// Calculate the histograms for the grayscale images
	calcHist(&(*imgMat), 1, channels, Mat(), hist, 1, &histSize, ranges,
			uniform, false);
	Ptr<vector<int>> bins = makePtr<vector<int>>(1, 256);
	return makePtr<Histogram>(hist, bins, 1, img->getLabel(), false, false);
}

Ptr<Histogram> HistogramComparator::createHSVHistogram(
		const Ptr<Img> & img) {
	int h_bins = 50;
	int s_bins = 32;
	int histSize[] = { h_bins, s_bins };

	float h_ranges[] = { 0, 256 };
	float s_ranges[] = { 0, 180 };

	bool uniform = true; //bool accumulate = false;

	const float* ranges[] = { h_ranges, s_ranges };

	int channels[] = { 0, 1 };

	/// Histograms
	MatND hist;
	Mat imgMat;
	cvtColor(*img->getImage(), imgMat, COLOR_BGR2HSV);

	/// Calculate the histograms for the grayscale images
	calcHist(&imgMat, 1, channels, Mat(), hist, 2, histSize, ranges, uniform,
			false);
	//---------------CHECK THIS-------------//normalize( hist, hist, 0, 1, NORM_MINMAX, -1, Mat() );
	Ptr<vector<int>> bins = makePtr<vector<int>>();
	bins->push_back(50);
	bins->push_back(32);
	return makePtr<Histogram>(hist, bins, 2, img->getLabel(), false, false);
}

void HistogramComparator::pmakeAndSaveLandscape(char mode, string label,
		bool saveToFile) {
	int size = mode & HistogramComparator::HSV ? 50 : 256;
	int channels =
			mode & HistogramComparator::HSV ?
					2 : (mode & HistogramComparator::COLOR ? 3 : 1);
	Mat minMaxHist = Mat::zeros(size, channels * 3, CV_32SC1);
	Img* current;
	Ptr<vector<int>> bins = makePtr<vector<int>>();
	vector<int> maxValues;
	if (mode & HistogramComparator::COLOR) {
		bins->push_back(256);
		bins->push_back(256);
		bins->push_back(256);
		maxValues.push_back(256);
		maxValues.push_back(256);
		maxValues.push_back(256);
	} else if (mode & HistogramComparator::HSV) {
		bins->push_back(50);
		bins->push_back(32);
		maxValues.push_back(256);
		maxValues.push_back(180);
	} else {
		bins->push_back(256);
		maxValues.push_back(256);
	}
	Ptr<vector<Mat>> hists = makePtr<vector<Mat>>();
	bool firstPass = true;
	Ptr<Histogram> result = makePtr<Histogram>(minMaxHist, bins, channels,
			label, true, true);
	int count = 0;
	for (uint p = 0; p < this->patterns->size(); p++) {
		current = this->patterns->at(p)->getImage();
		hists->clear();
		vector<Mat> layers;
		Ptr<Mat> src = makePtr<Mat>();
		if (mode & HistogramComparator::COLOR) {
			src = current->getImage();
		} else if (mode & HistogramComparator::GRAY) {
			src = current->getGrayImg();
		} else {
			cvtColor(*current->getImage(), *src, COLOR_BGR2HSV);
		}
		split(*src, layers);
		for (int c = 0; c < channels; c++) {
			Mat hist;
			float range[] = { 0, maxValues[c] };
			const float* ranges[] = { range };
			calcHist(&layers[c], 1, 0, Mat(), hist, 1, &bins->at(c), ranges,
					true, false);
			int normVal = maxValues.at(c); //mode & HistogramComparator::COLOR ? 256 : 1;
			normalize(hist, hist, 0, normVal, NORM_MINMAX, -1, Mat());
			hists->push_back(hist);
		}
		update_minMax(minMaxHist, hists, bins, maxValues, channels, firstPass);
		firstPass = false;
		count++;
	}
	update_average(minMaxHist, bins, channels, count);
	char saveMode = mode & 7;
	saveMode = saveMode | HistogramsIO::LANDSCAPE;
	if (mode & YAML) {
		saveMode = saveMode | HistogramsIO::YAML;
	} else {
		saveMode = saveMode | HistogramsIO::XML;
	}
	Ptr<vector<Ptr<Histogram>>> output = makePtr<vector<Ptr<Histogram>>> ();
	output->push_back(result);
	this->io->save(output, saveMode, saveToFile);
}

HistogramComparator::~HistogramComparator() {
}

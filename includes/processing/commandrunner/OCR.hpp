/*
 * OCR.hpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef OCR_HPP_
#define OCR_HPP_

#include "opencv2/text.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include <processing/commandrunner/CommandRunner.hpp>
#include <tesseract/publictypes.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;
using namespace cv::text;

namespace Anakin {

//ERStat extraction is done in parallel for different channels
class Parallel_extractCSER: public cv::ParallelLoopBody {
private:
	vector<Mat> &channels;
	vector<vector<ERStat> > &regions;
	vector<Ptr<ERFilter> > er_filter1;
	vector<Ptr<ERFilter> > er_filter2;

public:
	Parallel_extractCSER(vector<Mat> &_channels,
			vector<vector<ERStat> > &_regions,
			vector<Ptr<ERFilter> > _er_filter1,
			vector<Ptr<ERFilter> > _er_filter2) :
			channels(_channels), regions(_regions), er_filter1(_er_filter1), er_filter2(
					_er_filter2) {
	}

	virtual void operator()(const cv::Range &r) const {
		for (int c = r.start; c < r.end; c++) {
			er_filter1[c]->run(channels[c], regions[c]);
			er_filter2[c]->run(channels[c], regions[c]);
		}
	}
	Parallel_extractCSER & operator=(const Parallel_extractCSER &a);
};

//OCR recognition is done in parallel for different detections
template<class T>
class Parallel_OCR: public cv::ParallelLoopBody {
private:
	vector<Mat> &detections;
	vector<string> &outputs;
	vector<vector<Rect> > &boxes;
	vector<vector<string> > &words;
	vector<vector<float> > &confidences;
	vector<Ptr<T> > &ocrs;

public:
	Parallel_OCR(vector<Mat> &_detections, vector<string> &_outputs,
			vector<vector<Rect> > &_boxes, vector<vector<string> > &_words,
			vector<vector<float> > &_confidences, vector<Ptr<T> > &_ocrs) :
			detections(_detections), outputs(_outputs), boxes(_boxes), words(
					_words), confidences(_confidences), ocrs(_ocrs) {
	}

	virtual void operator()(const cv::Range &r) const {
		for (int c = r.start; c < r.end; c++) {
			ocrs[c % ocrs.size()]->run(detections[c], outputs[c], &boxes[c],
					&words[c], &confidences[c], OCR_LEVEL_WORD);
		}
	}
	Parallel_OCR & operator=(const Parallel_OCR &a);
};

class OCR: public CommandRunner {
public:
	OCR(const string & threadName);
	virtual ~OCR();

	void validateRequest(const Ptr<vector<string>> & input);

	void extendServerCommandsWith(const Ptr<Flags> & flags);

	void initializeCommandRunner(const Ptr<DataOutput> & out,
			const Ptr<SFBMCache> & cache);

	void run();

	Ptr<Help> getHelp();

	// simple version
	Ptr<vector<string>> detect(string & lastError);

	// complex version, but very precise
	Ptr<vector<string>> detect2(string & lastError);
private:
	I_CommunicationFormatter* cf;
	bool showWords = false;
	string lang = "eng";
	string datapath = "";
	string scenesDir = "landscapes/forest";
	tesseract::OcrEngineMode mode = tesseract::OEM_TESSERACT_ONLY;

	//--- ocr2 vars ---
	bool downsize = false;
	int REGION_TYPE = 1;
	int GROUPING_ALGORITHM = 0;
	int RECOGNITION = 0;

	vector<Mat> channels;
	vector<vector<ERStat> > regions; //two channels

	vector<Ptr<ERFilter> > er_filters1;
	vector<Ptr<ERFilter> > er_filters2;
	vector<Ptr<OCRHMMDecoder> > decoders;
	vector<Ptr<OCRTesseract> > ocrs;

	int num_ocrs = 10;
	//--- methods for OCR ---

	bool isRepetitive(const string& s);

	void er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions,
			vector<Vec2i> group, Mat& segmentation);

};

}
/* namespace Anakin */

#endif /* OCR_HPP_ */

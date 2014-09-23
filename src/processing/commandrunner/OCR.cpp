/*
 * OCR.cpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#include <tesseract/baseapi.h>
#include <processing/commandrunner/OCR.hpp>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <output/DataOutput.hpp>
#include <output/JSON.h>
#include <output/JSONValue.h>
#include <processing/Flags.hpp>
#include <sys/types.h>
#include <utils/Constants.hpp>
#include <utils/help/HelpOCR.hpp>
#include <sstream>
#include <utils/ClearVector.hpp>

using namespace std;
using namespace cv;
using namespace cv::text;

namespace Anakin {

OCR::OCR(const string & threadName) :
		CommandRunner("OCR_Demo", threadName) {
	this->cf = new CommunicationFormatterJSON();
}

OCR::~OCR() {
	delete cf;
}

Help* OCR::getHelp() {
	return new HelpOCR();
}

void OCR::initializeCommandRunner(const Ptr<DataOutput> & out,
		const Ptr<SFBMCache> & cache) {
	CommandRunner::initializeCommandRunner(out, cache);

	//OCR flags
	flags->setOptionalFlag("ocr");
	flags->setOptionalFlag("reqID");
	flags->setOptionalFlag("lang");
	flags->setOptionalFlag("datapath");
	flags->setOptionalFlag("mode");
	flags->setOptionalFlag("words");
	flags->setDependence("words", "ocr");
	flags->setDependence("lang", "ocr");
	flags->setDependence("datapath", "ocr");
	flags->setDependence("mode", "ocr");

	flags->setVerbose(true);

	//Initialize OCR engine (we initialize 10 instances in order to work several recognitions in parallel)
	cout << "Initializing OCR engines on thread " << this->getThreadName()
			<< "..." << endl;

	downsize = false;
	REGION_TYPE = 1;
	GROUPING_ALGORITHM = 0;
	RECOGNITION = 0;

	regions = vector<vector<ERStat> >(2);

//	char *region_types_str[2] = { const_cast<char *>("ERStats"),
//			const_cast<char *>("MSER") };
//	char *grouping_algorithms_str[2] = {
//			const_cast<char *>("exhaustive_search"),
//			const_cast<char *>("multioriented") };
//	char *recognitions_str[2] = { const_cast<char *>("Tesseract"),
//			const_cast<char *>("NM_chain_features + KNN") };

// Create ERFilter objects with the 1st and 2nd stage default classifiers
// since er algorithm is not reentrant we need one filter for channel

	for (int i = 0; i < 2; i++) {
		Ptr<ERFilter> er_filter1 =
				createERFilterNM1(
						loadClassifierNM1(
								"/home/franco/Librerias/opencv_contrib/modules/text/samples/trained_classifierNM1.xml"),
						8, 0.00015f, 0.13f, 0.2f, true, 0.1f);
		Ptr<ERFilter> er_filter2 =
				createERFilterNM2(
						loadClassifierNM2(
								"/home/franco/Librerias/opencv_contrib/modules/text/samples/trained_classifierNM2.xml"),
						0.5);
		er_filters1.push_back(er_filter1);
		er_filters2.push_back(er_filter2);
	}

	//double t_r = getTickCount();

	//Initialize OCR engine (we initialize 10 instances in order to work several recognitions in parallel)
	//cout << "Initializing OCR engines ..." << endl;
	num_ocrs = 10;

	for (int o = 0; o < num_ocrs; o++) {
		ocrs.push_back(OCRTesseract::create());
	}

	Mat transition_p;
	string filename = "OCRHMM_transitions_table.xml";
	FileStorage fs(filename, FileStorage::READ);
	fs["transition_probabilities"] >> transition_p;
	fs.release();
	Mat emission_p = Mat::eye(62, 62, CV_64FC1);
	string voc =
			"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

	for (int o = 0; o < num_ocrs; o++) {
		decoders.push_back(
				OCRHMMDecoder::create(
						loadOCRHMMClassifierNM(
								"/home/franco/Librerias/opencv_contrib/modules/text/samples/OCRHMM_knn_model_data.xml.gz"),
						voc, transition_p, emission_p));
	}

	cout << "Thread " << this->getThreadName() << " Done!" << endl;
}

void OCR::validateRequest(const Ptr<vector<string>> & input) {
	reqID = "";
	if (flags->validateInput(input)) {
		Ptr<vector<string>> values;

		if (flags->flagFound(Constants::PARAM_REQID)) {
			values = flags->getFlagValues(Constants::PARAM_REQID);
			if (values->size() != 1) {
				lastError = "flag " + Constants::PARAM_REQID
						+ " expects only one value";
				inputError = true;
				return;
			}
			reqID = values->at(0);
		}
		if (flags->flagFound("ocr")) {
			values = flags->getFlagValues("ocr");
			if (values->size() != 1) {
				lastError = "flag ocr expects only one value";
				inputError = true;
				return;
			}
			scenesDir = values->at(0);
		}

		if (flags->flagFound("words")) {
			showWords = true;
		} else {
			showWords = false;
		}

		if (flags->flagFound("mode")) {
			values = flags->getFlagValues("mode");
			if (values->size() == 1) {
				int ocrMode = stoi(values->at(0));
				if (ocrMode < 0 || ocrMode > 3) {
					lastError = "param mode use values from 0 to 3";
					inputError = true;
					return;
				}

				switch (ocrMode) {
				case 0:
					this->mode = tesseract::OEM_TESSERACT_ONLY;
					break;
				case 1:
					this->mode = tesseract::OEM_CUBE_ONLY;
					break;
				case 2:
					this->mode = tesseract::OEM_TESSERACT_CUBE_COMBINED;
					break;
				case 3:
					this->mode = tesseract::OEM_DEFAULT;
					break;
				}
			} else {
				lastError = "flag mode expects only one value";
				inputError = true;
				return;
			}
		} else {
			this->mode = tesseract::OEM_TESSERACT_ONLY;
		}

		if (flags->flagFound("lang")) {
			values = flags->getFlagValues("lang");
			if (values->size() == 1) {
				lang = stoi(values->at(0));
			} else {
				lastError = "param lang need only one value";
				inputError = true;
				return;
			}
		} else {
			lang = "eng";
		}

		if (flags->flagFound("datapath")) {
			values = flags->getFlagValues("datapath");
			if (values->size() == 1) {
				datapath = values->at(0);
			} else {
				lastError = "flag datapath expects only one value";
				inputError = true;
				return;
			}
		} else {
			datapath = "/usr/share/tesseract-ocr/";
		}

		// DO NOT DELETE VALUES! there are alias to flags content!
		//delete values;
	} else {
		lastError = "input error!";
		inputError = true;
	}
}

Ptr<vector<string>> OCR::detect(string & lastError) {
	//TODO change location??
	tesseract::TessBaseAPI* api = new tesseract::TessBaseAPI();

	if (api->Init(this->datapath.c_str(), this->lang.c_str(), this->mode)) {
		lastError = "Could not initialize tesseract";
		return Ptr<vector<string>>();
	}

	cv::Mat img = cv::imread(scenesDir);

	if (!img.data) {
		lastError = "Error loading image";
		return Ptr<vector<string>>();
	}

	api->SetImage((uchar*) img.data, img.size().width, img.size().height,
			img.channels(), img.step1());
	api->Recognize(0);
	Ptr<vector<string>> result = makePtr<vector<string>>();

	tesseract::ResultIterator* ri = api->GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;

	if (ri != 0) {
		stringstream text;
		do {
			const char* word = ri->GetUTF8Text(level);
			float conf = ri->Confidence(level);
			int x1, y1, x2, y2;
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			if (showWords) {
				stringstream s;
				s << "word: \"" << word << "\" \tconf: " << conf
						<< " \tBoundingBox: " << x1 << "," << y1 << "," << x2
						<< "," << y2 << ";";
				result->push_back(s.str());
			}
			text << word << " ";
			delete[] word;
		} while (ri->Next(level));
		string fullText = text.str();
		if (!fullText.empty())
			fullText.pop_back();
		result->push_back(fullText);
	}

	api->Clear();
	api->End();
	delete api;
	return result;
}

bool OCR::isRepetitive(const string& s) {
	int count = 0;
	int count2 = 0;
	int count3 = 0;
	int first = (int) s[0];
	int last = (int) s[(int) s.size() - 1];
	for (int i = 0; i < (int) s.size(); i++) {
		if ((s[i] == 'i') || (s[i] == 'l') || (s[i] == 'I'))
			count++;
		if ((int) s[i] == first)
			count2++;
		if ((int) s[i] == last)
			count3++;
	}
	if ((count > ((int) s.size() + 1) / 2) || (count2 == (int) s.size())
			|| (count3 > ((int) s.size() * 2) / 3)) {
		return true;
	}

	return false;
}

void OCR::er_draw(vector<Mat> &channels, vector<vector<ERStat> > &regions,
		vector<Vec2i> group, Mat& segmentation) {
	for (int r = 0; r < (int) group.size(); r++) {
		ERStat er = regions[group[r][0]][group[r][1]];
		if (er.parent != NULL) // deprecate the root region
		{
			int newMaskVal = 255;
			int flags = 4 + (newMaskVal << 8) + FLOODFILL_FIXED_RANGE
					+ FLOODFILL_MASK_ONLY;
			floodFill(channels[group[r][0]], segmentation,
					Point(er.pixel % channels[group[r][0]].cols,
							er.pixel / channels[group[r][0]].cols), Scalar(255),
					0, Scalar(er.level), Scalar(0), flags);
		}
	}
}

Ptr<vector<string>> OCR::detect2(string & lastError) {

	Mat frame, grey;

	//cout << "TIME_OCR_INITIALIZATION_ALT = "<< ((double)getTickCount() - t_r)*1000/getTickFrequency() << endl;

	frame = cv::imread(scenesDir);

	double t_all = (double) getTickCount();

	if (downsize)
		resize(frame, frame, Size(320, 240));

	/*Text Detection*/
	channels.clear();

	cvtColor(frame, grey, COLOR_RGB2GRAY);
	//grey.copyTo(orig_grey);
	// Extract channels to be processed individually
	channels.push_back(grey);

	Mat inv;
	grey.convertTo(inv, grey.type(), -1, 255);
	channels.push_back(inv);

	regions[0].clear();
	regions[1].clear();
	//double t_d = (double)getTickCount();

	switch (REGION_TYPE) {
	case 0: {
		parallel_for_(cv::Range(0, (int) channels.size() - 1),
				Parallel_extractCSER(channels, regions, er_filters1,
						er_filters2));
		break;
	}
	case 1: {
		//Extract MSER
		vector<vector<Point> > contours;
		MSER(21, (int) (0.00002 * grey.cols * grey.rows),
				(int) (0.05 * grey.cols * grey.rows), 1, 0.7)(grey, contours);

		//Convert the output of MSER to suitable input for the grouping/recognition algorithms
		if (contours.size() > 0)
			MSERsToERStats(grey, contours, regions);

		break;
	}
	case 2: {
		break;
	}
	}
	//cout << "TIME_REGION_DETECTION_ALT = " << ((double)getTickCount() - t_d)*1000/getTickFrequency() << endl;

	// Detect character groups
	//double t_g = getTickCount();
	vector<vector<Vec2i> > nm_region_groups;
	vector<Rect> nm_boxes;
	switch (GROUPING_ALGORITHM) {
	case 0: {
		erGrouping(frame, channels, regions, nm_region_groups, nm_boxes,
				ERGROUPING_ORIENTATION_HORIZ);
		break;
	}
	case 1: {
		erGrouping(frame, channels, regions, nm_region_groups, nm_boxes,
				ERGROUPING_ORIENTATION_ANY,
				"/home/franco/Librerias/opencv_contrib/modules/text/samples/trained_classifier_erGrouping.xml",
				0.5);
		break;
	}
	}
	//cout << "TIME_GROUPING_ALT = " << ((double)getTickCount() - t_g)*1000/getTickFrequency() << endl;

	/*Text Recognition (OCR)*/

	Ptr<vector<string>> words_detection = makePtr<vector<string>>();
	float min_confidence1 = 0.f, min_confidence2 = 0.f;

	if (RECOGNITION == 0) {
		min_confidence1 = 51.f;
		min_confidence2 = 60.f;
	}

	vector<Mat> detections;

	//t_r = getTickCount();

	for (int i = 0; i < (int) nm_boxes.size(); i++) {
		Mat group_img = Mat::zeros(frame.rows + 2, frame.cols + 2, CV_8UC1);
		er_draw(channels, regions, nm_region_groups[i], group_img);
		group_img(nm_boxes[i]).copyTo(group_img);
		copyMakeBorder(group_img, group_img, 15, 15, 15, 15, BORDER_CONSTANT,
				Scalar(0));
		detections.push_back(group_img);
	}
	vector<string> outputs((int) detections.size());
	vector<vector<Rect> > boxes((int) detections.size());
	vector<vector<string> > words((int) detections.size());
	vector<vector<float> > confidences((int) detections.size());

	// parallel process detections in batches of ocrs.size() (== num_ocrs)
	for (int i = 0; i < (int) detections.size(); i = i + (int) num_ocrs) {
		Range r;
		if (i + (int) num_ocrs <= (int) detections.size())
			r = Range(i, i + (int) num_ocrs);
		else
			r = Range(i, (int) detections.size());

		switch (RECOGNITION) {
		case 0:
			parallel_for_(r,
					Parallel_OCR<OCRTesseract>(detections, outputs, boxes,
							words, confidences, ocrs));
			break;
		case 1:
			parallel_for_(r,
					Parallel_OCR<OCRHMMDecoder>(detections, outputs, boxes,
							words, confidences, decoders));
			break;
		}
	}

	stringstream text;
	for (int i = 0; i < (int) detections.size(); i++) {

		outputs[i].erase(remove(outputs[i].begin(), outputs[i].end(), '\n'),
				outputs[i].end());
		//cout << "OCR output = \"" << outputs[i] << "\" lenght = " << outputs[i].size() << endl;
		if (outputs[i].size() < 3)
			continue;

		for (int j = 0; j < (int) boxes[i].size(); j++) {
			boxes[i][j].x += nm_boxes[i].x - 15;
			boxes[i][j].y += nm_boxes[i].y - 15;

			//cout << "  word = " << words[j] << "\t confidence = " << confidences[j] << endl;
			if ((words[i][j].size() < 2)
					|| (confidences[i][j] < min_confidence1)
					|| ((words[i][j].size() == 2)
							&& (words[i][j][0] == words[i][j][1]))
					|| ((words[i][j].size() < 4)
							&& (confidences[i][j] < min_confidence2))
					|| isRepetitive(words[i][j]))
				continue;

			text << words[i][j] << " ";
		}

	}
	string fullText = text.str();
	if (!fullText.empty())
		fullText.pop_back();
	words_detection->push_back(fullText);

	//cout << "TIME_OCR_ALT = " << ((double)getTickCount() - t_r)*1000/getTickFrequency() << endl;

	t_all = ((double) getTickCount() - t_all) * 1000 / getTickFrequency();

	return words_detection;
}

void OCR::run() {

	if (inputError) {
		this->out->error(
				this->cf->outputError(
						CommunicationFormatterJSON::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}
	int ireqID = stoi(reqID);

	string lastError;

	Ptr<vector<string>> results = detect2(lastError);
	Ptr<vector<Ptr<wstring>>> jsonresults = makePtr<vector<Ptr<wstring>>>();

	if (results.get() == NULL) {
		this->out->error(
				this->cf->outputError(
						I_CommunicationFormatter::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}

	jsonresults->push_back(this->cf->format(results));

	this->out->output(
			this->cf->outputResponse(reqID, I_CommunicationFormatter::CF_OCR,
					jsonresults), ireqID);
}

} /* namespace Anakin */

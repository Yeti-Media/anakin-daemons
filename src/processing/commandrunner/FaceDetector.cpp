// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * FaceDetector.cpp
 *
 *  Created on: 13/02/2015
 *      Author: Paul Fitzpatrick
 */

#include <processing/commandrunner/FaceDetector.hpp>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <utils/Constants.hpp>
#include <opencv2/objdetect.hpp>

namespace Anakin {

FaceDetector::FaceDetector(const string & threadName) :
		CommandRunner("FaceDetector", threadName) {
	cf = new CommunicationFormatterJSON();
	cascade = new CascadeClassifier;
	maxDim = 0;
}

FaceDetector::~FaceDetector() {
	delete cascade;
	delete cf;
}

void FaceDetector::validateRequest(const Ptr<vector<string>> & input) {
	reqID = "";
	imgPath = "";
	inputError = true;
	if (flags->validateInput(input)) {
		inputError = false;
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
		if (flags->flagFound("detect_face")) {
			values = flags->getFlagValues("detect_face");
			if (values->size() != 1) {
				stringstream f;
				int i = 0;
				for (vector<string>::iterator it = values->begin();
						it != values->end(); ++it) {
					i++;
					f << i << ") " << *it << " ";
				}
				lastError =
						"flag img expects only one value, and the input was: "
								+ f.str();
				inputError = true;
				return;
			}
			imgPath = values->at(0);
		}
	}
}

void FaceDetector::extendServerCommandsWith(const Ptr<Flags> & serverFlags) {
	serverFlags->setRequiredFlag("face_cascade");
	serverFlags->setOptionalFlag("max_dim");
}

void FaceDetector::parseServerFlags(const Ptr<Flags> & serverFlags) {
	Ptr<vector<string>> values;
	if (serverFlags->flagFound("face_cascade")) {
		values = serverFlags->getFlagValues("face_cascade");
		if (values->size() == 1) {
			cascadePath = values->at(0);
		} else {
			cout << "param face_cascade needs only one value" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (serverFlags->flagFound("max_dim")) {
		values = serverFlags->getFlagValues("max_dim");
		if (values->size() == 1) {
			maxDim = stoi(values->at(0));
		} else {
			cout << "param max_dim needs only one value" << endl;
			exit(EXIT_FAILURE);
		}
	} else {
		maxDim = 1024;
	}
}

void FaceDetector::initializeCommandRunner(const Ptr<DataOutput> & out,
		const Ptr<SFBMCache> & cache) {
	CommandRunner::initializeCommandRunner(out, cache);
	flags->setOptionalFlag("reqID");
	flags->setRequiredFlag("detect_face");

    if( !cascade->load(cascadePath)) {
		cout << "could not load face_cascade from " << cascadePath << endl;
		exit(EXIT_FAILURE);
    }
}

vector<Rect> FaceDetector::detect(const cv::Mat& img, double scale) {
	vector<Rect> faces;
	Mat gray, smallImg(cvRound(img.rows/scale),
					   cvRound(img.cols/scale),
					   CV_8UC1);
    cvtColor(img,gray,COLOR_BGR2GRAY);
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
    equalizeHist(smallImg, smallImg);
	cascade->detectMultiScale(smallImg, faces,
							  1.1, 2, CASCADE_SCALE_IMAGE,
							  Size(30, 30));
	return faces;
}

void FaceDetector::run() {
	if (inputError) {
		this->out->error(
				this->cf->outputError(
						CommunicationFormatterJSON::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}
	int ireqID = stoi(reqID);

	if (lastError != "") {
		this->out->error(
				this->cf->outputError(
						I_CommunicationFormatter::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}

	double scale = 1.0;
	Mat img = cv::imread(imgPath);
	int w = img.size().width;
	int h = img.size().height;
	vector<Rect> faces;
	if (w!=0 && h!=0 && maxDim!=0) {
		double dim = fmax(w,h);
		if (dim>maxDim) scale = dim/maxDim;
		faces = detect(img,scale);
	}
	JSONObject root;
	JSONArray array;
	for (auto face : faces) {
		JSONObject rect;
		rect[L"x"] = new JSONValue((double)((int)(face.x*scale)));
		rect[L"y"] = new JSONValue((double)((int)(face.y*scale)));
		rect[L"h"] = new JSONValue((double)((int)(face.height*scale)));
		rect[L"w"] = new JSONValue((double)((int)(face.width*scale)));
		array.push_back(new JSONValue(rect));
	}
	root[L"faces"] = new JSONValue(array);
	JSONValue *value = new JSONValue(root);
	Ptr<wstring> q = makePtr<wstring>(value->Stringify());
	delete value;
	this->out->output(q,ireqID);
}

Ptr<Help> FaceDetector::getHelp() {
	Ptr<Help> x;
	return x;
}

} /* namespace Anakin */

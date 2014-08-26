/*
 * OCRDemo.cpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <output/DataOutput.hpp>
#include <output/JSON.h>
#include <output/JSONValue.h>
#include <processing/commandrunner/OCRDemo.hpp>
#include <processing/Flags.hpp>
#include <processing/ocr/OCRDetector.hpp>
#include <sys/types.h>
#include <utils/Constants.hpp>
#include <utils/help/HelpOCRDemo.hpp>
#include <sstream>

namespace Anakin {

OCRDemo::OCRDemo() :
		CommandRunner() {
	this->cf = new CommunicationFormatterJSON();
}

OCRDemo::~OCRDemo() {
	delete cf;
}

string OCRDemo::getProgramName() {
	return "OCR_Demo";
}

Help* OCRDemo::getHelp() {
	return new HelpOCRDemo();
}

void OCRDemo::initializeCommandRunner(DataOutput* out, SFBMCache* cache) {
	CommandRunner::initializeCommandRunner(out, cache);

	flags->setOverridingFlag("ocrDemo");
	flags->setOverridingFlag("ocrAdvDemo");
	flags->setOptionalFlag(Constants::PARAM_REQID);

	//OCR flags
	flags->setOptionalFlag("ocr");
	flags->setOptionalFlag("rois");
	flags->setOptionalFlag("reqID");
	flags->setDependence("rois", "ocr");
	flags->setOptionalFlag("clearEvery");
	flags->setDependence("clearEvery", "ocr");
	flags->setOptionalFlag("lang");
	flags->setOptionalFlag("datapath");
	flags->setOptionalFlag("mode");
	flags->setDependence("lang", "ocr");
	flags->setDependence("datapath", "ocr");
	flags->setDependence("mode", "ocr");

	flags->setNoValuesFlag("show");
	vector<string>* showLooseDeps = new vector<string>(0);
	showLooseDeps->push_back("landscape");
	showLooseDeps->push_back("ocr");
	showLooseDeps->push_back("face");
	flags->setLooseDependencies("show", showLooseDeps);

	flags->setVerbose(true);
}

void OCRDemo::validateRequest(vector<string> *input) {
	reqID = "";
	if (flags->validateInput(input)) {
		vector<string>* values = NULL;

		if (flags->flagFound("ocrDemo")) {
			run_ocr_demo = true;
			return;
		}
		if (flags->flagFound("ocrAdvDemo")) {
			run_ocr_adv_demo = true;
			return;
		}
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
			run_ocr_detect = true;
		}
		if (flags->flagFound("reqID")) {
			values = flags->getFlagValues("reqID");
			if (values->size() != 1) {
				lastError = "flag reqID expects only one value";
				inputError = true;
				return;
			}
			reqID = values->at(0);
		}
		if (flags->flagFound("rois")) {
			values = flags->getFlagValues("rois");
			if (values->size() > 0 && values->size() % 4 == 0) {
				for (uint v = 0; v < values->size(); v += 4) {
					int p1x = stof(values->at(v));
					int p1y = stof(values->at(v + 1));
					int p2x = stof(values->at(v + 2));
					int p2y = stof(values->at(v + 3));
					cv::Point* p1 = new cv::Point(p1x, p1y);
					cv::Point* p2 = new cv::Point(p2x, p2y);
					std::pair<cv::Point*, cv::Point*> roi(p1, p2);
					ocrRois.push_back(roi);
				}
				histSafeOffset = stof(values->at(0));
			} else {
				lastError = "flag rois expects only one value";
				inputError = true;
				return;
			}
		}

		if (flags->flagFound("show")) {
			show = true;
		}

		if (flags->flagFound("clearEvery")) {
			values = flags->getFlagValues("clearEvery");
			if (values->size() == 1) {
				clearEvery = stoi(values->at(0));
			} else {
				lastError = "flag clearEvery expects only one value";
				inputError = true;
				return;
			}
		}

		if (flags->flagFound("mode")) {
			values = flags->getFlagValues("mode");
			if (values->size() == 1) {
				mode = stoi(values->at(0));
				if (mode < 0 || mode > 3) {
					lastError = "param mode use values from 0 to 3";
					inputError = true;
					return;
				}
			} else {
				lastError = "flag mode expects only one value";
				inputError = true;
				return;
			}
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
		}
		// DO NOT DELETE VALUES! there are alias to flags content!
		//delete values;
	} else {
		lastError = "input error!";
		inputError = true;
	}
}

void OCRDemo::run() {

	if (inputError) {
		this->out->error(
				this->cf->outputError(
						CommunicationFormatterJSON::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}
	int ireqID = stoi(reqID);

	if (run_ocr_demo) {
		OCRDetector ocrDetector("eng", "ocrTest.png");
		ocrDetector.basic_demo();
		return;
	}

	if (run_ocr_adv_demo) {
		OCRDetector ocrDetector("eng", "ocrTest.png");
		ocrDetector.demo();
		return;
	}

	datapath = "/usr/share/tesseract-ocr/";
	OCRDetector ocrDetector(scenesDir, datapath, lang,
			ocrMode);
	vector<string>* results = ocrDetector.detect(&ocrRois, show, clearEvery);
	vector<wstring*> jsonresults;


	jsonresults.push_back(this->cf->format(results));
	wcout << "resultado ocr" << *jsonresults.at(0) << endl;

	this->out->output(
			this->cf->outputResponse(reqID, I_CommunicationFormatter::CF_OCR,
					jsonresults), ireqID);
}

} /* namespace Anakin */
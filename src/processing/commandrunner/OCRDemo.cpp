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
#include <utils/ClearVector.hpp>

namespace Anakin {

OCRDemo::OCRDemo() :
		CommandRunner("OCR_Demo") {
	this->cf = new CommunicationFormatterJSON();
}

OCRDemo::~OCRDemo() {
	delete cf;
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
	flags->setOptionalFlag("reqID");
	flags->setOptionalFlag("lang");
	flags->setOptionalFlag("datapath");
	flags->setOptionalFlag("mode");
	flags->setDependence("lang", "ocr");
	flags->setDependence("datapath", "ocr");
	flags->setDependence("mode", "ocr");

	flags->setVerbose(true);
}

void OCRDemo::validateRequest(vector<string> *input) {
	reqID = "";
	if (flags->validateInput(input)) {
		vector<string>* values = NULL;

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

		if (flags->flagFound("mode")) {
			values = flags->getFlagValues("mode");
			if (values->size() == 1) {
				ocrMode = stoi(values->at(0));
				if (ocrMode < 0 || ocrMode > 3) {
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

void OCRDemo::run() {

	if (inputError) {
		this->out->error(
				this->cf->outputError(
						CommunicationFormatterJSON::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}
	int ireqID = stoi(reqID);

	string lastError;

	OCRDetector ocrDetector(scenesDir, datapath, lang, ocrMode);
	vector<string>* results = ocrDetector.detect(lastError);
	vector<wstring*> jsonresults;

	if (results == NULL) {
		this->out->error(
				this->cf->outputError(
						I_CommunicationFormatter::CF_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		delete results;
		return;
	}

	jsonresults.push_back(this->cf->format(results));

	this->out->output(
			this->cf->outputResponse(reqID, I_CommunicationFormatter::CF_OCR,
					jsonresults), ireqID);

	for_each( jsonresults.begin(), jsonresults.end(), delete_pointer_element<wstring*>());
	delete results;
}

} /* namespace Anakin */

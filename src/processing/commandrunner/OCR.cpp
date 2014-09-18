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

namespace Anakin {

OCR::OCR() :
		CommandRunner("OCR_Demo") {
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

	flags->setOverridingFlag("ocrDemo");
	flags->setOverridingFlag("ocrAdvDemo");
	flags->setOptionalFlag(Constants::PARAM_REQID);

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

	Ptr<vector<string>> results = detect(lastError);
	Ptr<vector<Ptr<wstring>>> jsonresults = makePtr<vector<Ptr<wstring>>>();

	if (results == NULL) {
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

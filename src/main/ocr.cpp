// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Author: Paul Fitzpatrick
 *
 * It isn't quite clear where the version of anakin expected by rubix
 * is defined or built. I've patched something together from bits and 
 * pieces.
 *
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <utils/files/Files.hpp>
#include <processing/commandrunner/OCR.hpp>
#include <algorithm>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <test/acceptance/TestTools.hpp>
#include <processing/Program.hpp>
#include <connection/Daemon.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

void SHOW_ERROR(const std::string& txt) {
	fprintf(stderr,"%s", txt.c_str());
}

int main(int argc, char *argv[]) {
	if (argc==1 || 
		(argc==2 && string(argv[1])=="-help") || 
		(argc==2 && string(argv[1])=="--help")) {
		printf("To run completely standalone, find trained_classifierNM1.xml etc., and do:\n");
		printf("  <program> -ocr image.jpg -classifier path/to/classifier/files\n");
		printf("To run with an already-started server (at port 8080 for example), do:\n");
		printf("  <program> -ocr image.jpg -url http://localhost:8080\n");
		printf("To start as a server, do:\n");
		printf("  <program> -serve 8080 -classifier path/to/classifier/files\n");
		return 0;
	}

	Ptr<vector<string>> input = makePtr<vector<string>>();
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}
	Flags flags;
	flags.setOptionalFlag("classifier");
	flags.setOptionalFlag("url");
	flags.setOptionalFlag("ocr");
	flags.setOptionalFlag("serve");
	if (!flags.validateInput(input)) return 1;

	Ptr<DataOutput> output = makePtr<DataOutput>();

	OCR* ocr = new OCR("OCR thread");
	Ptr<SFBMCache> nullCache;

	if (flags.getFlagValues("url")->size() == 1) {
		string cmd = "{\"action\":\"ocr\", \"ocr\":\""
			+ flags.getFlagValues("ocr")->at(0) + "\"}";
		string result;
		string err;
		cURL_JSON(flags.getFlagValues("url")->at(0),
				  cmd,result,err);
		if (err!="") {
			fprintf(stderr,"%s\n",err.c_str());
			return 1;
		}
		printf("%s\n",result.c_str());
		return 0;
	}

	if (flags.getFlagValues("classifier")->size() != 1) {
		fprintf(stderr,"Need exactly one -classifier directory, or a -url\n");
		return 1;
	}

	auto classifier = boost::filesystem::path(flags.getFlagValues("classifier")->at(0));

	boost::filesystem::path trained_classifierNM1 = classifier
			/ "trained_classifierNM1.xml";
	boost::filesystem::path trained_classifierNM2 = classifier
			/ "trained_classifierNM2.xml";
	boost::filesystem::path OCRHMM_transitions_table = classifier
			/ "OCRHMM_transitions_table.xml";
	boost::filesystem::path OCRHMM_knn_model_data = classifier
			/ "OCRHMM_knn_model_data.xml.gz";
	boost::filesystem::path classifier_erGrouping = classifier
			/ "trained_classifier_erGrouping.xml";


	if (!fs::exists(trained_classifierNM1)) {
		SHOW_ERROR(
				"Classifier: \"" + trained_classifierNM1.string()
						+ "\" does not exist\n");
	}

	if (!fs::exists(trained_classifierNM2)) {
		SHOW_ERROR(
				"Classifier: \"" + trained_classifierNM2.string()
						+ "\" does not exist\n");
	}

	if (!fs::exists(OCRHMM_transitions_table)) {
		SHOW_ERROR(
				"Classifier: \"" + OCRHMM_transitions_table.string()
						+ "\" does not exist\n");
	}

	if (!fs::exists(OCRHMM_knn_model_data)) {
		SHOW_ERROR(
				"Classifier: \"" + OCRHMM_knn_model_data.string()
						+ "\" does not exist\n");
	}

	if (!fs::exists(classifier_erGrouping)) {
		SHOW_ERROR(
				"Classifier: \"" + classifier_erGrouping.string()
						+ "\" does not exist\n");
	}

	Ptr<vector<string>> inputs = makePtr<vector<string>>();
	inputs->push_back("-classifierNM1");
	inputs->push_back(trained_classifierNM1.string());
	inputs->push_back("-classifierNM2");
	inputs->push_back(trained_classifierNM2.string());
	inputs->push_back("-OCRHMMtransitions");
	inputs->push_back(OCRHMM_transitions_table.string());
	inputs->push_back("-OCRHMMknn");
	inputs->push_back(OCRHMM_knn_model_data.string());
	inputs->push_back("-classifier_erGrouping");
	inputs->push_back(classifier_erGrouping.string());

	if (flags.getFlagValues("serve")->size() == 1) {
		inputs->push_back("-iHTTP");
		inputs->push_back(flags.getFlagValues("serve")->at(0));
		inputs->push_back("-oHTTP");
		inputs->push_back("-threads");
		inputs->push_back("1");
		Daemon<OCR> *daemon = new Daemon<OCR>();
		daemon->useDatabase(false); // OCR does not need database
		int result = daemon->start(inputs);
		delete daemon;
		return result;
	}

	ocr->extendServerCommandsWith(ocr->getProgramFlags());
	ocr->validateRequest(inputs);
	ocr->parseServerFlags(ocr->getProgramFlags());
	ocr->initializeCommandRunner(output, nullCache);
	inputs->push_back("-ocr");
	inputs->push_back(flags.getFlagValues("ocr")->at(0));
	ocr->validateRequest(inputs);

	string lastError;
	Ptr<vector<string>> results = ocr->detect2(lastError);

	if (results.get() == NULL) {
		SHOW_ERROR("OCR fail: " + lastError + "\n");
		exit(1);
	}

	if (results->empty()) {
		SHOW_ERROR("No results found\n");
		exit(1);
	}

	CommunicationFormatterJSON cf;
	auto out = cf.format(results);
	wcout << *out;

	return 0;
}


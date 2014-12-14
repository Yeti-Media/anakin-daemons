/*
 * TestDirParserForTestDirOCR..cpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <utils/files/Files.hpp>
#include <processing/commandrunner/OCR.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

/**
 * Used for test case common initialization (Test directory).
 */
struct PathFixtureOCRDetectorTest {

	boost::filesystem::path testDir;

	PathFixtureOCRDetectorTest() {
		BOOST_TEST_MESSAGE("fixture setup");

		int argc = boost::unit_test::framework::master_test_suite().argc;
		char** argv = boost::unit_test::framework::master_test_suite().argv;

		if (argc < 2) {
			BOOST_FAIL("First param must be a testing directory.\n");
		}

		string dir;
		//BOOST bug fixing about space and dirs
		for (int i = 1; i < argc; i++) {
			dir.append(argv[i]);
			if (i < argc - 1) {
				dir.append(" ");
			}
		}

		testDir = boost::filesystem::path(dir);
		BOOST_REQUIRE_MESSAGE(fs::is_directory(testDir),
				"Test directory " + testDir.string() + " not found");
	}
	~PathFixtureOCRDetectorTest() {
		BOOST_TEST_MESSAGE("teardown fixture");
	}

};

//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( OCRDetectorSuite, PathFixtureOCRDetectorTest)

BOOST_FIXTURE_TEST_CASE( basicTest, PathFixtureOCRDetectorTest ) {

	boost::filesystem::path image = testDir / "examples" / "ocr"
			/ "dataset_images" / "img_88.png";

	boost::filesystem::path classifier = testDir / "examples" / "ocr"
			/ "classifier";

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


	if (!fs::exists(image)) {
		BOOST_FAIL("Image: \"" + image.string() + "\" does not exists");
	}

	if (!fs::exists(trained_classifierNM1)) {
		BOOST_FAIL(
				"Classifier: \"" + trained_classifierNM1.string()
						+ "\" does not exists");
	}

	if (!fs::exists(trained_classifierNM2)) {
		BOOST_FAIL(
				"Classifier: \"" + trained_classifierNM2.string()
						+ "\" does not exists");
	}

	if (!fs::exists(OCRHMM_transitions_table)) {
		BOOST_FAIL(
				"Classifier: \"" + OCRHMM_transitions_table.string()
						+ "\" does not exists");
	}

	if (!fs::exists(OCRHMM_knn_model_data)) {
		BOOST_FAIL(
				"Classifier: \"" + OCRHMM_knn_model_data.string()
						+ "\" does not exists");
	}

	if (!fs::exists(classifier_erGrouping)) {
		BOOST_FAIL(
				"Classifier: \"" + classifier_erGrouping.string()
						+ "\" does not exists");
	}

	string lastError;

	Ptr<DataOutput> output = makePtr<DataOutput>();

	OCR* ocr = new OCR("OCR thread");
	Ptr<SFBMCache> nullCache;

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

	ocr->extendServerCommandsWith(ocr->getProgramFlags());
	ocr->validateRequest(inputs);
	ocr->parseServerFlags(ocr->getProgramFlags());
	ocr->initializeCommandRunner(output, nullCache);
	inputs->push_back("-ocr");
	inputs->push_back(image.string());
	ocr->validateRequest(inputs);

	Ptr<vector<string>> results = ocr->detect2(lastError);

	if (results.get() == NULL) {
		BOOST_FAIL("OCR fail: " + lastError);
	}

	if (results->empty()) {
		BOOST_FAIL("No results found");
	}

	bool PREMIER = false;
	bool APARTMENTS = false;
	for (vector<string>::iterator word = results->begin();
			word != results->end(); ++word) {
		//Analyzing output
		BOOST_MESSAGE(*word);
		if (word->find("PREMIER") != string::npos) {
			PREMIER = true;
		}
		if (word->find("APARTMENTS") != string::npos) {
			APARTMENTS = true;
		}
	}

	if (!PREMIER) {
		BOOST_FAIL("Word \"PREMIER\" not found");
	}
	if (!APARTMENTS) {
		BOOST_FAIL("Word \"APARTMENTS\" not found");
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

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

	boost::filesystem::path img_3 = testDir / "examples" / "ocr"
			/ "dataset_images" / "img_3.png";

	if (!fs::exists(img_3)) {
		BOOST_FAIL("Image: \"" + img_3.string() + "\" does not exists");
	}

	string lastError;

	Ptr<DataOutput> output = makePtr<DataOutput>();

	OCR* ocr = new OCR("OCR thread");
	Ptr<SFBMCache> nullCache;
	ocr->initializeCommandRunner(output, nullCache);

	Ptr<vector<string>> inputs = makePtr<vector<string>>();
	inputs->push_back("-ocr");
	inputs->push_back(img_3.string());

	ocr->validateRequest(inputs);
	Ptr<vector<string>> results = ocr->detect2(lastError);

	if (results.get() == NULL) {
		BOOST_FAIL("OCR fail: " + lastError);
	}

	if (results->empty()) {
		BOOST_FAIL("No results found");
	}

	bool specialFound = false;
	bool specialTShirt = false;
	for (vector<string>::iterator word = results->begin();
			word != results->end(); ++word) {
		//Analyzing output
		if (word->find("Special") != string::npos) {
			specialFound = true;
		}
		if (word->find("T-Shirt") != string::npos) {
			specialTShirt = true;
		}
	}

	if (!specialFound) {
		BOOST_FAIL("Word \"Special\" not found");
	}
	if (!specialTShirt) {
		BOOST_FAIL("Word \"T-Shirt\" not found");
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

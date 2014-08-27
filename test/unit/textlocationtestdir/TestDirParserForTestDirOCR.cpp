/*
 * TestDirParserForTestDirOCR..cpp
 *
 *  Created on: 26/8/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

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
#include <utils/Files.hpp>
#include <test/utils/TextLocator.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

/**
 * Used for test case common initialization (Test directory).
 */
struct PathFixtureOCR {

	boost::filesystem::path testDir;

	PathFixtureOCR() {
		BOOST_TEST_MESSAGE("setup fixture");

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

		testDir = dir;
		BOOST_REQUIRE_MESSAGE(fs::is_directory(testDir),
				"Test directory " << testDir << " not found");
	}
	~PathFixtureOCR() {
		BOOST_TEST_MESSAGE("teardown fixture");
	}

};

//____________________________________________________________________________//

BOOST_FIXTURE_TEST_SUITE( TextLocatorSuite, PathFixtureOCR)

BOOST_FIXTURE_TEST_CASE( basicTest, PathFixtureOCR ) {

	boost::filesystem::path filesToImport = testDir / "examples" / "ocr"
			/ "text_localization";

	if (!fs::exists(filesToImport)) {
		BOOST_FAIL("Dir: \"" + filesToImport.string() + "\" does not exists");
	}

	TextLocator locator(filesToImport);


	//------------- FIRST TEST --------------------
	boost::filesystem::path testFile1 = filesToImport / "gt_img_362.txt";
	list<LocatedWord> locations = locator.getLocations(testFile1);
	bool testResultOK = false;

	for (list<LocatedWord>::iterator actualWord = locations.begin();
			actualWord != locations.end(); ++actualWord) {
		if (((*actualWord).word.compare("1.000.000") == 0)
				&& ((*actualWord).x1.compare("61") == 0)
				&& ((*actualWord).y1.compare("82") == 0)
				&& ((*actualWord).x2.compare("146") == 0)
				&& ((*actualWord).y2.compare("95") == 0)) {
			testResultOK = true;
		}
	}

	if (!testResultOK) {
		BOOST_FAIL("Fist test failed");
	}

	//------------- SECOND TEST --------------------
	testFile1 = filesToImport / "gt_img_403.txt";
	locations = locator.getLocations(testFile1);
	testResultOK = false;

	for (list<LocatedWord>::iterator actualWord = locations.begin();
			actualWord != locations.end(); ++actualWord) {
		if (((*actualWord).word.compare("Quarter,") == 0)
				&& ((*actualWord).x1.compare("51") == 0)
				&& ((*actualWord).y1.compare("125") == 0)
				&& ((*actualWord).x2.compare("97") == 0)
				&& ((*actualWord).y2.compare("136") == 0)) {
			testResultOK = true;
		}
	}

	if (!testResultOK) {
		BOOST_FAIL("Second test failed");
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

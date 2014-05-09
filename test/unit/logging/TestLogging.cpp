/*
 * TestLogging.cpp
 *
 *  Created on: 30/04/2014
 *      Author: Franco Pellegrini
 */
#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_MODULE Logging_Test_Module
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/test/included/unit_test.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <iostream>
#include <string>
#include <regex>
#include <utils/Files.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

	/**
	 * Used for test case common initialization (Test directory).
	 */
	struct PathFixture {

		boost::filesystem::path testDir;

		PathFixture() {
			BOOST_TEST_MESSAGE("setup fixture");

			if (boost::unit_test::framework::master_test_suite().argc != 2) {
				BOOST_FAIL("First param must be a testing directory.\n");
			}

			testDir = boost::unit_test::framework::master_test_suite().argv[1];
			BOOST_REQUIRE_MESSAGE(fs::is_directory(testDir),
					"Test directory " << testDir << " not found");
		}
		~PathFixture() {
			BOOST_TEST_MESSAGE("teardown fixture");
		}

	};

//____________________________________________________________________________//

	BOOST_FIXTURE_TEST_SUITE( acceptance , PathFixture)

	BOOST_FIXTURE_TEST_CASE( file_output, PathFixture ) {

		boost::filesystem::path logFile = testDir / "anakin.log";

		if (remove(logFile.c_str()) != 0)
		BOOST_FAIL("Error deleting file");

		//logger initialization
		if (!Logging::OutputPolicyFile::SetFileStream(logFile.string())) {
			BOOST_FAIL("Fail to open the logging file.");
		}

		LOG_F("Info")<< "test - 12345\n 1239\n\n";

		//TODO make a pattern for time
		std::regex regexLOG(".*- Info: \ttest - 12345\n 1239\n\n\n");
		std::string capture = get_file_contents(logFile.c_str());
		if (!std::regex_match(capture, regexLOG)) {
			BOOST_FAIL("Wrong console output.");
		}

		LOG_F("Info")<< "test - 12345\n 1239\n\n";

		//TODO make a pattern for time
		std::regex regexLOG2(".*- Info: \ttest - 12345\n 1239\n\n\n.*- Info: \ttest - 12345\n 1239\n\n\n");
		capture = get_file_contents(logFile.c_str());
		if (!std::regex_match(capture, regexLOG2)) {
			BOOST_FAIL("Wrong console output.");
		}
	}

	BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/
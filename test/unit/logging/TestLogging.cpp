/*
 * TestLogging.cpp
 *
 *  Created on: 30/04/2014
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
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <iostream>
#include <string>
#include <regex>
#include <utils/files/Files.hpp>

namespace fs = boost::filesystem;
using namespace Anakin;

namespace Testing {

//____________________________________________________________________________//

BOOST_AUTO_TEST_SUITE( LoggingSuite )

BOOST_AUTO_TEST_CASE( file_output ) {

	boost::filesystem::path dirOutput("/tmp/ram/Anakin/UnitTesting/Logging");

	boost::filesystem::path logFile = dirOutput / "UnitTestLogging.log";

	if (!fs::exists(dirOutput)) {
		fs::create_directories(dirOutput);
	}

	if (fs::exists(logFile)) {
		if (remove(logFile.c_str()) != 0) {
			BOOST_FAIL("Error deleting file");
		}
	}

	//logger initialization
	if (!Logging::OutputPolicyFile::SetFileStream(logFile.string())) {
		BOOST_FAIL("Fail to open the logging file.");
	}

	LOG_F("Info")<< "test - 12345\n 1239\n\n";

	//TODO make a pattern for time
	std::regex regexLOG(".*- Info: \ttest - 12345\n 1239\n\n\n");
	Ptr<std::string> capture = get_file_contents(logFile.string());
	if (!std::regex_match(*capture, regexLOG)) {
		BOOST_FAIL("Wrong console output.");
	}

	LOG_F("Info")<< "test - 12345\n 1239\n\n";

	//TODO make a pattern for time
	std::regex regexLOG2(
			".*- Info: \ttest - 12345\n 1239\n\n\n.*- Info: \ttest - 12345\n 1239\n\n\n");
	capture = get_file_contents(logFile.string());
	if (!std::regex_match(*capture, regexLOG2)) {
		BOOST_FAIL("Wrong console output.");
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

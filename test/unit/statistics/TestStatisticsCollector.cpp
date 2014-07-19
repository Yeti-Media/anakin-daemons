/*
 * TestLogging.cpp
 *
 *  Created on: 30/04/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_UNIT_TESTING

#define BOOST_TEST_DYN_LINK
#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE Main
#endif
#include <boost/test/unit_test.hpp>

#include <cstdio>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <utils/statistics/StatisticsCollector.hpp>
#include <iostream>
#include <string>
#include <regex>

using namespace Anakin;

namespace Testing {

BOOST_AUTO_TEST_SUITE(statisticsCollector)

BOOST_AUTO_TEST_CASE(basic_test) {
	StatisticsCollector collector;

	collector.addItem("comando_1", 23.2);

	std::regex regexLOG(".*- Info: \ttest - 12345\n 1239\n\n\n");
	std::string capture = collector.compute();
	if (!std::regex_match(capture, regexLOG)) {
		BOOST_FAIL("Wrong console output.");
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

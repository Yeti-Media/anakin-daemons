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

BOOST_AUTO_TEST_SUITE(StatisticsCollectorSuite)

BOOST_AUTO_TEST_CASE(basic_test) {
	StatisticsCollector collector;

	collector.addItem("comando_1", "Test1", 23.2);

	string expected =
				"==================================\n"
				"|       Command Results          |\n"
				"==================================\n"
				"Worst 23.2 ms. | Best 23.2 ms. | Avg 23.2 ms. | Executed 1 times | Command: comando_1\n"
				"\n"
				"==================================\n"
				"|        Groups Results          |\n"
				"==================================\n"
				"Worst 23.2 ms. | Best 23.2 ms. | Avg 23.2 ms. | Executed 1 times | Group: Test1\n";
	std::string capture = collector.compute();
	if (capture.compare(expected) != 0) {
		string msj = "\nWrong console output 1:\n" + capture;
		BOOST_FAIL(msj);
	}

	collector.addItem("comando_1", "Test2", 25);
	collector.addItem("comando_1", "Test2", 100);
	collector.addItem("comando_1", "Test2", 2);

	expected =
				"==================================\n"
				"|       Command Results          |\n"
				"==================================\n"
				"Worst 100 ms. | Best 2 ms. | Avg 37.55 ms. | Executed 4 times | Command: comando_1\n"
				"\n"
				"==================================\n"
				"|        Groups Results          |\n"
				"==================================\n"
				"Worst 23.2 ms. | Best 23.2 ms. | Avg 23.2 ms. | Executed 1 times | Group: Test1\n"
				"Worst 100 ms. | Best 2 ms. | Avg 42.3333 ms. | Executed 3 times | Group: Test2\n";

	capture = collector.compute();
	if (capture.compare(expected) != 0) {
		string msj = "\nWrong console output 2:\n" + capture;
		BOOST_FAIL(msj);
	}

	collector.addItem("comando_2", "Test3", 23.2);
	collector.addItem("comando_2", "Test3", 23.1);
	collector.addItem("comando_2", "Test3", 23.2);

	expected =
				"==================================\n"
				"|       Command Results          |\n"
				"==================================\n"
				"Worst 100 ms. | Best 2 ms. | Avg 37.55 ms. | Executed 4 times | Command: comando_1\n"
				"Worst 23.2 ms. | Best 23.1 ms. | Avg 23.1667 ms. | Executed 3 times | Command: comando_2\n"
				"\n"
				"==================================\n"
				"|        Groups Results          |\n"
				"==================================\n"
				"Worst 23.2 ms. | Best 23.2 ms. | Avg 23.2 ms. | Executed 1 times | Group: Test1\n"
				"Worst 100 ms. | Best 2 ms. | Avg 42.3333 ms. | Executed 3 times | Group: Test2\n"
				"Worst 23.2 ms. | Best 23.1 ms. | Avg 23.1667 ms. | Executed 3 times | Group: Test3\n";

	capture = collector.compute();
	if (capture.compare(expected) != 0) {
		string msj = "\nWrong console output 3:\n" + capture;
		BOOST_FAIL(msj);
	}
}

BOOST_AUTO_TEST_SUITE_END()

//____________________________________________________________________________//

};
//namespace Testing

#endif  /*COMPILE_MODE == COMPILE_FOR_UNIT_TESTING*/

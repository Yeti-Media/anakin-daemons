/*
 * main.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <test/acceptance/TestTools.hpp>
#include <test/acceptance/TestDefinitions.hpp>

int main(int argc, const char * argv[]) {
	StatisticsCollector collector;
	testingDirCheck(argc, argv);

	//patternSimpleTest(argc, argv, &collector);
	//patternBenchmarkTest(argc, argv, &collector);
	//printStatistics(&collector);

	ocrBenchmarkTest(argc, argv, &collector);
	printStatistics(&collector, "OCR");

	exitWithSucces();
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/


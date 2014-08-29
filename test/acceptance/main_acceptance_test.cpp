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
	testingDirCheck(argc, argv);

	//patternSimpleTest(argc, argv);
	//patternBenchmarkTest(argc, argv);
	ocrBenchmarkTest(argc, argv);

	exitWithSucces();
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/


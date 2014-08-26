/*
 * TestDefinitions.hpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef TESTDEFINITIONS_HPP_
#define TESTDEFINITIONS_HPP_

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <utils/statistics/StatisticsCollector.hpp>

/**
 * Files and Directory's validation
 */
void testingDirCheck(int argc, const char * argv[]);

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin.
 */
void patternSimpleTest(int argc, const char * argv[], StatisticsCollector* collector);

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin, but with heavy examples, to test performance.
 */
void patternBenchmarkTest(int argc, const char * argv[],
		StatisticsCollector* collector);

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin, but with heavy examples, to test performance.
 */
void ocrBenchmarkTest(int argc, const char * argv[],
		StatisticsCollector* collector);

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* TESTDEFINITIONS_HPP_ */

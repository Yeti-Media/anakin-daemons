/*
 * CompileConfigurations.hpp
 *
 *  Created on: 24/04/2014
 *      Author: Franco Pellegrini
 */

#ifndef COMPILECONFIGURATIONS_HPP_
#define COMPILECONFIGURATIONS_HPP_

/**
 * Uncomment what project do you want to compile. Comment the others.
 */
#define PATTERNMATCHING
//#define MATCHERCACHE
//#define DBCONNECTOR
//#define EXTRACTOR
//#define TRAINER

/**
 * If this is uncommented, Unit Testing is enabled an the main is removed.
 * Use this feature only AFTER compilation, because it needs a functional binary.
 */
#define UNIT_TEST

#ifdef PATTERNMATCHING
	int patternMatching(int argc, const char * argv[]);
#endif
#ifdef MATCHERCACHE
	int matcherCache(int argc, const char * argv[]);
#endif
#ifdef DBCONNECTOR
	int dbConnector(int argc, const char * argv[]);
#endif
#ifdef EXTRACTOR
	int extractor(int argc, const char * argv[]);
#endif
#ifdef TRAINER
	int trainer(int argc, const char * argv[]);
#endif

#endif /* COMPILECONFIGURATIONS_HPP_ */

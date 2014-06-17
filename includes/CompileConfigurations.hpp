/*
 * CompileConfigurations.hpp
 *
 *  Created on: 24/04/2014
 *      Author: Franco Pellegrini
 */
#ifndef COMPILECONFIGURATIONS_HPP_
#define COMPILECONFIGURATIONS_HPP_

#include "processing/Flags.hpp"
#include <boost/filesystem/path.hpp>

/**
 * Constants for compiling options (do not modify)
 */
#define COMPILE_FOR_PRODUCTION 0
#define COMPILE_FOR_UNIT_TESTING 1
#define COMPILE_FOR_BIN_ACCEPTANCE_TESTING 2
#define COMPILE_FOR_README_UPDATE 3

#define PATTERNMATCHER 1
#define PATTERNDBCONNECTOR 2
#define PATTERNEXTRACTOR 3
#define PATTERNTRAINER 4

#define FACEMATCHER 101
#define FACEDBCONNECTOR 102
#define FACEEXTRACTOR 103
#define FACETRAINER 104

#define MATCHERCACHETEST 5000

#define ALLMODULES 1000000

/** ======================================================================
 *  ======================================================================
 *  +++++++++++  CUSTOMIZE YOUR COMPILE OPTIONS FROM HERE  +++++++++++++++
 *  ======================================================================
 *  ======================================================================
 */

/** ======================================================================
 *  Select a Compilation mode:
 *
 *	    1) COMPILE_MODE_PRODUCTION: compile only the selected subprogram
 *	       ready for production. You have to choose one subprogram later on this
 *	       file.
 *
 *	    2) COMPILE_FOR_UNIT_TESTING: generate a Unit Testing bin that can
 *	       run testing on a COMPILE_MODE_TESTING bin.
 *	       The first param must be a valid path to a testing directory.
 *	       For example:
 *
 *	       anakin "/home/franco/Trabajos/Yeti-Media/TestDir/"
 *
 *	    3) COMPILE_FOR_BIN_ACCEPTANCE_TESTING: generate an Acceptance Testing bin
 *	       that can run binary/program testing on a COMPILE_MODE_TESTING bin.
 *	       The first param must be a valid path to a testing directory.
 *	       For example:
 *
 *	       anakin "/home/franco/Trabajos/Yeti-Media/TestDir/"
 *
 *	    4) COMPILE_FOR_README_UPDATE: export all help files into a specific dir.
 *
 *  ======================================================================
 */
//#define COMPILE_MODE COMPILE_FOR_PRODUCTION
//#define COMPILE_MODE COMPILE_FOR_UNIT_TESTING
#define COMPILE_MODE COMPILE_FOR_BIN_ACCEPTANCE_TESTING
//#define COMPILE_MODE COMPILE_FOR_README_UPDATE

#if COMPILE_MODE == COMPILE_FOR_PRODUCTION
/** ======================================================================
 *  If you choose COMPILE_FOR_PRODUCTION in COMPILE_MODE you can
 *  select ONLY ONE specific module to compile from this list:
 *  ======================================================================
 */

// *** PATTERN ****

//#define COMPILE_MODULE PATTERNMATCHER
//#define COMPILE_MODULE PATTERNDBCONNECTOR
#define COMPILE_MODULE PATTERNEXTRACTOR
//#define COMPILE_MODULE PATTERNTRAINER

// *** FACE RECOGNITION ****

//#define COMPILE_MODULE FACEMATCHER
//#define COMPILE_MODULE FACEDBCONNECTOR
//#define COMPILE_MODULE FACEEXTRACTOR
//#define COMPILE_MODULE FACETRAINER

// *** TESTS ***

//#define COMPILE_MODULE MATCHERCACHETEST

#else
#define COMPILE_MODULE ALLMODULES
#endif //COMPILE_MODE == COMPILE_FOR_PRODUCTION

#endif /* COMPILECONFIGURATIONS_HPP_ */

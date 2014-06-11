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

#define PATTERNMATCHING 0
#define MATCHERCACHE 1
#define DBCONNECTOR 2
#define EXTRACTOR 3
#define TRAINER 4

#define ALLMODULES 100

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
 *  ======================================================================
 */
//#define COMPILE_MODE COMPILE_FOR_PRODUCTION
#define COMPILE_MODE COMPILE_FOR_UNIT_TESTING
//#define COMPILE_MODE COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#if COMPILE_MODE == COMPILE_FOR_PRODUCTION
/** ======================================================================
 *  If you choose COMPILE_FOR_PRODUCTION in COMPILE_MODE you can
 *  select ONLY ONE specific module to compile from this list:
 *
 *  PATTERNMATCHING
 *  MATCHERCACHE
 *  DBCONNECTOR
 *  EXTRACTOR
 *  TRAINER
 *  ======================================================================
 */
//#define COMPILE_MODULE PATTERNMATCHING
//#define COMPILE_MODULE MATCHERCACHE
//#define COMPILE_MODULE DBCONNECTOR
//#define COMPILE_MODULE EXTRACTOR
#define COMPILE_MODULE TRAINER

#else
#define COMPILE_MODULE ALLMODULES
#endif //COMPILE_MODE == COMPILE_FOR_PRODUCTION

#endif /* COMPILECONFIGURATIONS_HPP_ */

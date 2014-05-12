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
#define COMPILE_FOR_TESTING 1
#define COMPILE_FOR_UNIT_TESTING 2
#define COMPILE_FOR_BIN_ACCEPTANCE_TESTING 3

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
 *	    2) COMPILE_MODE_TESTING: it will configure and merge all the
 *	       subprograms in one bin (useful for unit testing). You need to
 *	       specify the subprogram to start through flags:
 *
 *	       anakin -modepatternmatching ...
 *	       anakin -modematchercache ...
 *	       anakin -modedbconnector ...
 *	       anakin -modeextractor ...
 *	       anakin -modetrainer ...
 *
 *	    3) COMPILE_FOR_UNIT_TESTING: generate a Unit Testing bin that can
 *	       run testing on a COMPILE_MODE_TESTING bin.
 *	       The first param must be a valid path to a testing directory.
 *	       For example:
 *
 *	       anakin "/home/franco/Trabajos/Yeti-Media/TestDir/"
 *
 *	    4) COMPILE_FOR_BIN_ACCEPTANCE_TESTING: generate an Acceptance Testing bin
 *	       that can run binary/program testing on a COMPILE_MODE_TESTING bin.
 *	       The first param must be a valid path to a testing directory.
 *	       For example:
 *
 *	       anakin "/home/franco/Trabajos/Yeti-Media/TestDir/"
 *
 *  ======================================================================
 */
#define COMPILE_MODE COMPILE_FOR_PRODUCTION
//#define COMPILE_MODE COMPILE_FOR_TESTING
//#define COMPILE_MODE COMPILE_FOR_UNIT_TESTING
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
#define COMPILE_MODULE PATTERNMATCHING
//#define COMPILE_MODULE MATCHERCACHE
//#define COMPILE_MODULE DBCONNECTOR
//#define COMPILE_MODULE EXTRACTOR
//#define COMPILE_MODULE TRAINER

#else
#define COMPILE_MODULE ALLMODULES
#endif //COMPILE_MODE == COMPILE_FOR_PRODUCTION

/** ======================================================================
 *  ======================================================================
 *  ++++++++  STOP CUSTOMIZING YOUR COMPILE OPTIONS FROM HERE  +++++++++++
 *  ======================================================================
 *  ======================================================================
 */

/** ----------------------------------------------------------------------
 * header for main functions
 *  ----------------------------------------------------------------------*/

namespace Anakin {

void initModuleFlags(Flags* flags);

#if COMPILE_MODULE == PATTERNMATCHING || COMPILE_MODULE == ALLMODULES
int patternMatching(int argc, const char * argv[]);
#endif

#if COMPILE_MODULE == MATCHERCACHE || COMPILE_MODULE == ALLMODULES
void showHelpMatcherCache();
int matcherCache(int argc, const char * argv[]);
#endif

#if COMPILE_MODULE == DBCONNECTOR || COMPILE_MODULE == ALLMODULES
void showHelpDbConnector();
int dbConnector(int argc, const char * argv[]);
#endif

#if COMPILE_MODULE == EXTRACTOR || COMPILE_MODULE == ALLMODULES
void showHelpExtractor();
int extractor(int argc, const char * argv[]);
#endif

#if COMPILE_MODULE == TRAINER || COMPILE_MODULE == ALLMODULES
void showHelpTrainer();
int trainer(int argc, const char * argv[]);
#endif

}

#endif /* COMPILECONFIGURATIONS_HPP_ */

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
#define COMPILE_MODE COMPILE_FOR_PRODUCTION
//#define COMPILE_MODE COMPILE_FOR_UNIT_TESTING
//#define COMPILE_MODE COMPILE_FOR_BIN_ACCEPTANCE_TESTING
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
//#define COMPILE_MODULE PATTERNEXTRACTOR
//#define COMPILE_MODULE PATTERNTRAINER
// *** FACE RECOGNITION ****
//#define COMPILE_MODULE FACEMATCHER
//#define COMPILE_MODULE FACEDBCONNECTOR
//#define COMPILE_MODULE FACEEXTRACTOR
//#define COMPILE_MODULE FACETRAINER
// *** OCR ***
#define COMPILE_MODULE OCRFULL

// *** TESTS ***

//#define COMPILE_MODULE MATCHERCACHETEST

#else
#define COMPILE_MODULE ALLMODULES
#endif //COMPILE_MODE == COMPILE_FOR_PRODUCTION

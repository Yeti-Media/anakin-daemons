/*
 * HelpOCRDemo.cpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#include <utils/help/HelpOCRDemo.hpp>

namespace Anakin {

HelpOCRDemo::HelpOCRDemo() {

	intro = "OCR demo to test.";
	usage =
			"  ./OCRDemo -ocr <path to image> [-mode <0-3> | -datapath <path> | -lang <[~]<lang_value>[+[~]<lang_value>]*>]\n"
					"  * NOTE: the order of the arguments doesn't matter (it only matters the order -flag [<values>])\n";
	flags =
			"-mode <0-3> :  sets which engine to use\n"
					"   OEM_TESSERACT_ONLY(0)          : Run Tesseract only - fastest\n"
					"   OEM_CUBE_ONLY(1)               : Run Cube only - better accuracy, but slower\n"
					"   OEM_TESSERACT_CUBE_COMBINED(2) : Run both and combine results - best accuracy\n"
					"   OEM_DEFAULT(3)                 : Specify this mode when calling init_*(),\n"
					"                                    to indicate that any of the above modes\n"
					"                                    should be automatically inferred from the\n"
					"                                    variables in the language-specific config,\n"
					"                                    command-line configs, or if not specified\n"
					"                                    in any of the above should be set to the\n"
					"                                    default OEM_TESSERACT_ONLY.\n"
					"-datapath <path> : the location of tessdata folder containing the trained data files\n"
					"-lang <[~]<lang_value>[+[~]<lang_value>]*> : sets the languages to use, ~ is used to override the loading of a language\n";
}

HelpOCRDemo::~HelpOCRDemo() {
	// TODO Auto-generated destructor stub
}

bool HelpOCRDemo::showDaemonHelp() {
	return true;
}

} /* namespace Anakin */

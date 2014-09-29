/*
 * HelpOCR.cpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#include <utils/help/HelpOCR.hpp>

namespace Anakin {

HelpOCR::HelpOCR() {

	intro = "OCR demo to test.";
	extendedDaemonFlags =
			"OCR Daemon arguments:\n"
			"-classifierNM1 <path> -classifierNM2 <path> -OCRHMMtransitions <path> -OCRHMMknn <path> [-numocrs <int>]\n\n"
			"Flags:\n\n"
					"-numocrs <int>				: number of OCR instances per thread\n"
					"-classifierNM1 <path>			: path to trained_classifierNM1.xml file.\n"
					"-classifierNM2 <path>			: path to trained_classifierNM2.xml file.\n"
					"-OCRHMMtransitions <path>		: path to OCRHMM_transitions_table.xml file.\n"
					"-OCRHMMknn <path>			: path to OCRHMM_knn_model_data.xml.gz file.\n"
			"\n";
	usage =
			"  ./OCR -ocr <path to image> [-mode <0-3> | -words | -datapath <path> | -lang <[~]<lang_value>[+[~]<lang_value>]*>]\n"
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
					"-words                            : return the list of recognized words and their boundary box.\n"
					"-datapath <path>                  : the location of tessdata folder containing the trained data files\n"
					"-lang <[~]<lang_value>[+[~]<lang_value>]*> : sets the languages to use, ~ is used to override the loading of a language\n";
}

HelpOCR::~HelpOCR() {
	// TODO Auto-generated destructor stub
}

bool HelpOCR::showDaemonHelp() {
	return true;
}

} /* namespace Anakin */

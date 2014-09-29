/*
 * HelpPatternExtractor.cpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#include <utils/help/HelpPatternExtractor.hpp>
#include <string>

namespace Anakin {

bool HelpPatternExtractor::showDaemonHelp() {
	return false;
}

HelpPatternExtractor::HelpPatternExtractor() {
	intro =
			"Extracts information from images, constructs histograms for histogram and landscape matching. Extracts keypoints and descriptors for images."
					"Labels are set using the image filename without the extension. For landscape is possible to set a custom label.\n";
	usage =
			"(I/O arguments) (paths arguments) ((landscape arguments)|(histograms arguments))"
					"(-landscape|-histograms|-patterns) (-iFile|iFolder <path>|-oPath <path>|-toJson) ((-label <-label>)|(-color|-gray|-hsv))\n";

	flags =
			"I/O arguments are:\n"
					"-landscape   : will construct histograms to use with landscape detection\n"
					"-histograms  : will construct one histogram per pattern\n"
					"-patterns    : will extract descriptors and keypoints\n"
					"\nPaths arguments are:\n"
					"-iFile|iFolder <path>  : sets the input as a file or folder path respectively\n"
					"-oPath <path>          : sets the path for where to store the serialized data\n"
					"-toJson                : this will make output serialized data to the console\n"
					"\nLandscape exclusive arguments are:\n"
					"-label <label>	: the label to use when serializing landscape histogram\n"
					"\nLandscape and histogram's common arguments are:\n"
					"-color : will use RGB to generate histograms and landscape\n"
					"-gray  : will use grayscale to generate histograms and landscape\n"
					"-hsv   : will use hue and saturation to generate histograms and landscape";
}

HelpPatternExtractor::~HelpPatternExtractor() {
	// TODO Auto-generated destructor stub
}


} /* namespace Anakin */

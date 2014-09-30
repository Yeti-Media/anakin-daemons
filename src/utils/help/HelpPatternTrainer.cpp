/*
 * HelpPatternTrainer.cpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#include <utils/help/HelpPatternTrainer.hpp>

namespace Anakin {

bool HelpPatternTrainer::showDaemonHelp() {
	return false;
}

HelpPatternTrainer::HelpPatternTrainer() {
	intro =
			"Loads patterns associated with a specific user and then it trains a SerializableFlannBasedMatcher. This program will generate two files for a matcher. An XML file will contain basic information and an IF file that will contain the trained index. Both files will be compressed.\n"
					"The patterns are loaded in ascending order using the ID attribute, the order in which the patterns are loaded are very important because when using a matcher, the matches refers to the matched images by their index.\n"
					"Keep in mind that training requires a good amount of time, it is preferred to construct several indexes instead of one really big one.\n";

	usage =
			"-user <userID>                     : the user's id, patterns from this user will be used to train the matcher\n"
					"-saveToFile [<folder>] <filename>  : two files will be created, folder+filename+.xml and folder+filename+.if\n"
					"-patternsId <id id ...>            : a spaced separated list of patterns id, which will be used to train the matcher\n";
}

HelpPatternTrainer::~HelpPatternTrainer() {
	// TODO Auto-generated destructor stub
}


} /* namespace Anakin */

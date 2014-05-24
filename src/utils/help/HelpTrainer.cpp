/*
 * HelpTrainer.cpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#include <utils/help/HelpTrainer.hpp>

namespace Anakin {

HelpTrainer::HelpTrainer() {
	usage = "-user <userID> -saveToFile [<folder>] <filename>\n"
			"-user <userID> -saveToFile [<folder>] <filename>\n"
			"-patternsId <id id ...> -saveToFile [<folder>] <filename>\n";
}

HelpTrainer::~HelpTrainer() {
	// TODO Auto-generated destructor stub
}

} /* namespace Anakin */

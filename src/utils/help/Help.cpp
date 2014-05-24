/*
 * Help.cpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#include <utils/help/Help.hpp>

namespace Anakin {

Help::Help() {

}

Help::~Help() {
}

std::string Help::getFullHelp() {
	return getIntro() + "\n" + getUsage() + "\n" + getFlags() + "\n"
			+ getExamples();
}

std::string Help::getIntro() {
	return "Summary:\n\n"+this->intro;
}

std::string Help::getUsage() {
	return "Usage:\n\n"+this->usage;
}

std::string Help::getFlags() {
	return "Flags:\n\n"+this->flags;
}

std::string Help::getExamples() {
	return "Examples:\n\n"+this->examples;
}

} /* namespace Anakin */

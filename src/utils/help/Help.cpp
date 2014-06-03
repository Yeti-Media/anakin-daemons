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
	return getIntro() + getUsage() + getFlags() + getExamples();
}

std::string Help::getIntro() {
	return "Summary:\n\n" + this->intro + "\n";
}

std::string Help::getUsage() {
	return "Usage:\n\n" + this->usage + "\n";
}

std::string Help::getFlags() {
	return "Flags:\n\n" + this->flags + "\n";
}

std::string Help::getExamples() {
	return "Examples:\n\n" + this->examples + "\n";
}

} /* namespace Anakin */

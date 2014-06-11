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
	if (!this->intro.empty()) {
		return "Summary:\n\n" + this->intro + "\n";
	} else {
		return "";
	}
}

std::string Help::getUsage() {
	if (!this->usage.empty()) {
		return "Usage:\n\n" + this->usage + "\n";
	} else {
		return "";
	}
}

std::string Help::getFlags() {
	if (!this->flags.empty()) {
		return "Flags:\n\n" + this->flags + "\n";
	} else {
		return "";
	}
}

std::string Help::getExamples() {
	if (!this->examples.empty()) {
		return "Examples:\n\n" + this->examples + "\n";
	} else {
		return "";
	}
}

} /* namespace Anakin */

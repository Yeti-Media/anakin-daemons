/*
 * Help.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELP_HPP_
#define HELP_HPP_

#include <string>

namespace Anakin {

class Help {
public:
	/**
	 * for simplicity, the constructor must init the properties of the class with
	 * the proper text.
	 */
	Help();
	virtual ~Help();

	/**
	 * return a full help text
	 */
	virtual std::string getFullHelp();

	/**
	 * return a summary of the program
	 */
	virtual std::string getIntro();

	/**
	 * return the usage of the program
	 */
	virtual std::string getUsage();

	/**
	 * return the documentation for every flag
	 */
	virtual std::string getFlags();

	/**
	 * return usage examples
	 */
	virtual std::string getExamples();
protected:
	std::string intro = "TODO\n";
	std::string usage = "TODO\n";
	std::string flags = "TODO\n";
	std::string examples = "TODO\n";
};

} /* namespace Anakin */

#endif /* HELP_HPP_ */

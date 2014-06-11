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

/**
 * Every concrete commandrunner must have an specific extension of this class.
 * The constructor should initialize intro (summary), usage (correct flags usage),
 * flags (every flag help), and examples, with the specific documentation.
 * A concrete commandrunner must have a static public member
 * named help (class Help) that contain a static object using a concrete class from
 * this class (Help).
 */
class Help {
public:
	/**
	 * For simplicity, the constructor must init the properties of the class with
	 * the proper documentation.
	 */
	Help();
	virtual ~Help();

	/**
	 * return a full help text
	 */
	std::string getFullHelp();

	/**
	 * return a summary of the program
	 */
	std::string getIntro();

	/**
	 * return the usage of the program
	 */
	std::string getUsage();

	/**
	 * return the documentation for every flag
	 */
	std::string getFlags();

	/**
	 * return usage examples
	 */
	std::string getExamples();
protected:
	std::string intro;
	std::string usage;
	std::string flags;
	std::string examples;
};

} /* namespace Anakin */

#endif /* HELP_HPP_ */

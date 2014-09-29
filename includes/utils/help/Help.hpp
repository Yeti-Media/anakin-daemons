/*
 * Help.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELP_HPP_
#define HELP_HPP_

#include <string>

using namespace std;

namespace Anakin {
/**
 * Every concrete program must provide an specific extension of this class.
 * The constructor should initialize intro (summary), usage (correct flags usage),
 * flags (every flag help), and examples, with the specific documentation.
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
	 * return true if you need to show daemon documentation
	 */
	virtual bool showDaemonHelp() = 0;

	/**
	 * return a full help text
	 */
	string getFullHelp();

	/**
	 * return a summary of the program
	 */
	string getIntro();

	/**
	 * return the usage of the program
	 */
	string getUsage();

	/**
	 * return the documentation for every flag
	 */
	string getFlags();

	/**
	 * return usage examples
	 */
	string getExamples();


protected:
	string daemonIntro;
	string intro;
	string usage;
	string flags;
	string examples;
	string extendedDaemonFlags;
};

} /* namespace Anakin */

#endif /* HELP_HPP_ */

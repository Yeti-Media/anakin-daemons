/*
 * Program.hpp
 *
 *  Created on: 12/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

#include <utils/help/Help.hpp>
#include <processing/Flags.hpp>
#include <string>
#include <vector>

using namespace std;

namespace Anakin {

class Program {
public:
	Program();
	virtual ~Program();

	/**
	 * Start the program. It initialize common vars and flags.
	 */
	int start(vector<string> *input);

	/**
	 * Get a full text manual for the concrete program
	 */
	string getFullTextHelp();

	/**
	 * Get a full text program name
	 */
	virtual string getProgramName() = 0;
protected:

	/**
	 * Initialize all used flags within the program
	 */
	virtual void initProgramFlags() = 0;

	/**
	 * Run the concrete program.
	 */
	virtual int run(vector<string> *input) = 0;

	/**
	 * Return a full help instance. Must be deleted after use.
	 */
	virtual Help* getHelp() = 0;

	Flags programFlags;
	bool verbose;
	string logFile;
};

} /* namespace Anakin */

#endif /* PROGRAM_HPP_ */

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

/**
 * Common abstract class for a program. A Program must provide a full help text and
 * a name. A program will configure 3 flags:
 *
 *   -help				: Provide a full help to cout.
 *   -verbose			: Set true to verbose var. Used to show more info at cout.
 *   -oLogFile	<file>	: Will configure the output log file, in the specified <file>.
 *   					  If there's no -oLogFile flag, no logging will be activated.
 *
 * A concrete class must implement:
 *
 *   void initProgramFlags()		: Add to programFlags all supported flags and
 *   							  		configurations for your program.
 *
 *   int run(vector<string> *input)	: Run the program with the flags parsed in input
 *   									vector.
 *
 *   string getProgramName()		: Get the name of the program.
 *
 *   Help* getHelp()				: Get an instance of a concrete Help class to use.
 *
 *   string getFullTextHelp()		: Template classes should override this method
 * 										(see Daemon.hpp as an example).
 */
class Program {
public:
	Program();
	virtual ~Program();

	/**
	 * Start the program. It initialize common vars and flags. User shoul delete the input.
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
	string tempDir;
};

} /* namespace Anakin */

#endif /* PROGRAM_HPP_ */

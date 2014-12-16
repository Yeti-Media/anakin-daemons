// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * Program.hpp
 *
 *  Created on: 12/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef PROGRAM_HPP_
#define PROGRAM_HPP_

#include <opencv2/core/cvstd.hpp>
#include <processing/Flags.hpp>
#include <utils/help/Help.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

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
	Program(const string & programName);
	virtual ~Program();

	/**
	 * Start the program. It initialize common vars and flags. User shoul delete the input.
	 */
	int start(const Ptr<vector<string>> & input);

	/**
	 * Start the program directly from command line arguments.
	 */
	int start(int argc, char *argv[]) {
		Ptr<vector<string>> input = makePtr<vector<string>>();
		for (int i = 1; i < argc; i++) {
			input->push_back(argv[i]);
		}
		return start(input);
	}

	/**
	 * Get a full text manual for the concrete program
	 */
	string getFullTextHelp();

	/**
	 * Get a full text program name
	 */
	string getProgramName();

	void setProgramName(const string & name);
protected:

	/**
	 * Initialize all used flags within the program
	 */
	virtual void initProgramFlags() = 0;

	/**
	 * Run the concrete program.
	 */
	virtual int run(const Ptr<vector<string>> & input) = 0;

	/**
	 * Return a full help instance. Must be deleted after use.
	 */
	virtual Ptr<Help> getHelp() = 0;

	Ptr<Flags> programFlags;
	bool verbose;
	string logFile;
	string tempDir;
private:
	string programName;
};

} /* namespace Anakin */

#endif /* PROGRAM_HPP_ */

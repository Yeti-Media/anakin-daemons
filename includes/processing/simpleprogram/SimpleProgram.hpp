/*
 * SimpleProgram.hpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef SIMPLEPROGRAM_HPP_
#define SIMPLEPROGRAM_HPP_

#include <utils/help/Help.hpp>
#include <processing/Flags.hpp>

namespace Anakin {

/**
 * This abstract class is used to implement and run programs without a server.
 */
class SimpleProgram {
public:
	SimpleProgram();

	virtual ~SimpleProgram();

	/**
	 * Excecute the program with argv artuments.
	 */
	int run(vector<string> *input);

	/**
	 * Return the name of the program.
	 */
	virtual string getProgramName() = 0;

	/**
	 * Get a full text manual for the concrete program
	 */
	string getFullTextHelp();

protected:

	/**
	 * Return a full help instance. Must be deleted after use.
	 */
	virtual Help* getHelp() = 0;

	/**
	 * Execute the concrete program.
	 */
	virtual int excecute(vector<string> *input) = 0;

	/**
	 * Initialize the concrete program used flags.
	 */
	virtual void setupProgramFlags() = 0;

	Flags* programFlags;
	bool verbose;
	string logFile;
};

} /* namespace Anakin */

#endif /* SIMPLEPROGRAM_HPP_ */

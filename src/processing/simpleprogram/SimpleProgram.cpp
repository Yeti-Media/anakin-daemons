/*
 * SimpleProgram.cpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#include <processing/simpleprogram/SimpleProgram.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace Anakin {

/**
 * Placeholder for inheritance.
 */
Help* SimpleProgram::getHelp() {
	return NULL;
}

SimpleProgram::SimpleProgram() {
	verbose = false;
	logFile = "";
	programFlags = new Flags();
}

/**
 * Placeholder for inheritance.
 */
void SimpleProgram::excecute(vector<string> *input) {
}

/**
 * Placeholder for inheritance.
 */
void SimpleProgram::setupProgramFlags() {

}

void SimpleProgram::run(vector<string> *input) {

	//______________________________________________________________________//
	//                                 FLAGS                                //
	//______________________________________________________________________//

	Flags::initTestingFlags(programFlags);

	programFlags->setOverridingFlag("help");
	programFlags->setNoValuesFlag("verbose");
	programFlags->setOptionalFlag("oLogFile");
	//programFlags->ignoreUnknownFlags(true);

	setupProgramFlags();

	//______________________________________________________________________//
	//                         FLAGS PARSING                                //
	//______________________________________________________________________//

	if (programFlags->validateInput(input)) {
		vector<string>* values = new vector<string>();

		if (programFlags->flagFound("help")) {
			Help* help = getHelp();
			cout << help->getFullHelp();
			delete help;
			exit(EXIT_SUCCESS);
		}
		if (programFlags->flagFound("verbose")) {
			verbose = true;
		}

		if (programFlags->flagFound("oLogFile")) {
			values->clear();
			values = programFlags->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << endl;
				exit(EXIT_FAILURE);
			}
		}
		delete values;
	} else {
		cout << "Input error!" << endl;
		exit(EXIT_FAILURE);
	}

	//______________________________________________________________________//
	//                         PROGRAM INITIALIZATION                        //
	//______________________________________________________________________//

	//logger initialization
	if (!logFile.empty()) {
		Logging::OutputPolicyFile::SetFileStream(logFile);
	}

	std::string startComand;
	for (uint i = 0; i < input->size(); i++) {
		if (i != 0) {
			startComand.append(" ");
		}
		if (input->at(i).find(" ")!=string::npos) {
			startComand.append("\"").append(input->at(i)).append("\"");
		} else {
			startComand.append(input->at(i));
		}
	}
	LOG_F("Args")<< startComand;

	excecute(input);
}

SimpleProgram::~SimpleProgram() {
	delete programFlags;
}

} /* namespace Anakin */

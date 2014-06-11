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

string SimpleProgram::getFullTextHelp() {
	Help* help = getHelp();
	string output = getProgramName() + "\n\n" + help->getFullHelp();
	delete help;
	return output;
}


/**
 * Placeholder for inheritance.
 */
Help* SimpleProgram::getHelp() {
	return NULL;
}

/**
 * Placeholder for inheritance.
 */
string SimpleProgram::getProgramName() {
	return "";
}

SimpleProgram::SimpleProgram() {
	verbose = false;
	logFile = "";
	programFlags = new Flags();
}

/**
 * Placeholder for inheritance.
 */
int SimpleProgram::excecute(vector<string> *input) {
	return 0;
}

/**
 * Placeholder for inheritance.
 */
void SimpleProgram::setupProgramFlags() {

}

int SimpleProgram::run(vector<string> *input) {

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
			cout << getFullTextHelp();
			return EXIT_SUCCESS;
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
				return EXIT_FAILURE;
			}
		}
		delete values;
	} else {
		cout << "Input error!" << endl;
		return EXIT_FAILURE;
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
		if (input->at(i).find(" ") != string::npos) {
			startComand.append("\"").append(input->at(i)).append("\"");
		} else {
			startComand.append(input->at(i));
		}
	}
	LOG_F("Args")<< startComand;

	excecute(input);
	return EXIT_SUCCESS;
}

SimpleProgram::~SimpleProgram() {
	delete programFlags;
}

} /* namespace Anakin */

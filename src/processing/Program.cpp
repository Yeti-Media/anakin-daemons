/*
 * Program.cpp
 *
 *  Created on: 12/06/2014
 *      Author: Franco Pellegrini
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <processing/Program.hpp>
#include <sys/types.h>
#include <utils/help/Help.hpp>
#include <cstdlib>
#include <iostream>

namespace fs = boost::filesystem;

namespace Anakin {

Program::Program() {
	verbose = false;
	fs::path temp("/tmp/ram/Anakin/" + getProgramName());
	if (!fs::is_directory(temp)) {
		fs::create_directories(temp);
	}
	tempDir = temp.string();
}

Program::~Program() {
}

void Program::initProgramFlags() {
}

int Program::start(vector<string> *input) {

	programFlags.setOverridingFlag("help");
	programFlags.setNoValuesFlag("verbose");
	programFlags.setOptionalFlag("oLogFile");

	initProgramFlags();

	//______________________________________________________________________//
	//                         FLAGS PARSING                                //
	//______________________________________________________________________//

	if (programFlags.validateInput(input)) {
		vector<string>* values = NULL;

		if (programFlags.flagFound("help")) {
			cout << this->getFullTextHelp();
			return EXIT_SUCCESS;
		}
		if (programFlags.flagFound("verbose")) {
			verbose = true;
		}

		if (programFlags.flagFound("oLogFile")) {
			values = programFlags.getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << endl;
				return EXIT_FAILURE;
			}
		}
		// delete values; DO NOT ADD THIS!! value is an alias from a flag content
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

	return run(input);
}

/**
 * inheritance placeholder
 */
int Program::run(vector<string> *input) {
	return 0;
}

/**
 * Placeholder for inheritance.
 */
Help* Program::getHelp() {
	return NULL;
}

string Program::getFullTextHelp() {
	Help* help = getHelp();
	string output = "\n" + getProgramName() + " Manual\n\n"
			+ help->getFullHelp();
	delete help;
	return output;
}

/**
 * inheritance placeholder
 */
string Program::getProgramName() {
	return "";
}

}
/* namespace Anakin */

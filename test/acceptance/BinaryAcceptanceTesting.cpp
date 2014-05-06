/*
 * BinaryAcceptanceTesting.cpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING
#include <string>
#include <iostream>
#include <stdlib.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <utils/Files.hpp>

using namespace std;
using namespace Anakin;
namespace fs = boost::filesystem;

/**
 * Files and Dir validation
 */
void testingDirCheck(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//  Directory validation
	//--------------------------------------------------------------
	if (argc != 2) {
		cerr << "First param must be a testing directory." << endl;
		exit(EXIT_FAILURE);
	}

	fs::path testDir = argv[1];
	if (!fs::is_directory(testDir)) {
		cerr << "Test directory " << testDir << " not found" << endl;
		exit(EXIT_FAILURE);
	}

	//--------------------------------------------------------------
	//  Files validation
	//--------------------------------------------------------------

	// ====== Anakin File =======
	fs::path anakinPath = testDir / "Anakin";

	if (!fs::is_regular_file(anakinPath)) {
		cerr << "Anakin bin " << anakinPath << " not found" << endl;
		exit(EXIT_FAILURE);
	}

	// ====== SQL script File =======
	fs::path sqlScriptPath = testDir / "script.sql";

	if (!fs::is_regular_file(sqlScriptPath)) {
		cerr << "SQL script " << sqlScriptPath << " not found" << endl;
		exit(EXIT_FAILURE);
	}

	string scriptContent = get_file_contents(sqlScriptPath.c_str());
	std::size_t found = scriptContent.find("DROP DATABASE IF EXISTS");
	if (found != std::string::npos) {
		cerr
				<< "Remove \"DROP DATABASE IF EXISTS ...\" and \"CREATE DATABASE ...\" from "
				<< sqlScriptPath << endl;
		exit(EXIT_FAILURE);
	}
}

int main(int argc, const char * argv[]) {
	string anakinDB = "AnakinAcceptanceTesting";

	testingDirCheck(argc, argv);

	//Testing database creation.
//	string command = "createdb " + anakinDB;
//	if (system(command.c_str())==0) {
//		ascasascac
//	}

	//psql -U username -d myDataBase -a -f myInsertFile

	cout << "Acceptance test passed" << endl;
	exit(EXIT_SUCCESS);
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

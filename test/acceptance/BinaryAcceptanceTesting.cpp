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
 * Execute a system call to a specific program
 */
void command(string command) {
	if (system(command.c_str()) != 0) {
		cerr << "Command \"" << command << "\" fail" << endl;
		cerr << "\n===============================================\n"
				<< "Acceptance test result: FAIL" << endl;
		exit(EXIT_FAILURE);
	}
}

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

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin.
 */
void simpleTest(int argc, const char * argv[]) {
	//testing database name
	string anakinDB = "AnakinAcceptanceTesting";
	//username for database
	string userDB = "postgres";

	fs::path testDir = argv[1];
	fs::path anakinPath = testDir / "Anakin";
	fs::path sqlScriptPath = testDir / "script.sql";

	//testing database cleanup
	command("dropdb --if-exists " + anakinDB);

	//testing database creation.
	command("createdb " + anakinDB);

	//run SQL script into database.
	command(
			"psql -U " + userDB + " -d " + anakinDB + " -q -f "
					+ sqlScriptPath.string());

}

int main(int argc, const char * argv[]) {

	testingDirCheck(argc, argv);

	simpleTest(argc, argv);

	cout << "Acceptance test result: SUCCESS" << endl;
	exit(EXIT_SUCCESS);
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

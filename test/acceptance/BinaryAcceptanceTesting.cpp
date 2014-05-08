/*
 * BinaryAcceptanceTesting.cpp
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING
//#define TESTING_DEBUGMODE

#include <string>
#include <iostream>
#include <stdlib.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <utils/Files.hpp>

using namespace std;
using namespace Anakin;
namespace fs = boost::filesystem;

void exitWithError() {
	cerr << "\n===============================================\n"
			<< "Acceptance test result: FAIL" << endl;
	exit(EXIT_FAILURE);
}

void exitWithSucces() {
	cout << "Acceptance test result: SUCCESS" << endl;
	exit(EXIT_SUCCESS);
}

/**
 * Execute a system call to a specific program
 */
void command(string command, bool showLogMsjIfFail = false) {
#ifdef TESTING_DEBUGMODE
	cout << "Command \"" << command << "\" executed" << endl;
#endif
	if (system(command.c_str()) != 0) {
		cerr << "Command \"" << command << "\" fail" << endl;
		if (showLogMsjIfFail) {
			cerr << "See log directory for program error outputs" << endl;
		}
		exitWithError();
	}
}

/**
 * Validate a directory existence.
 */
void validateDir(fs::path path, string msj) {
	if (!fs::is_directory(path)) {
		cerr << msj << " directory " << path << " not found" << endl;
		exitWithError();
	}
}

/**
 * Delete all dir content, except for the directory itself.
 */
void dirCleanup(fs::path path) {
	string pattern = path.string() + "/*";
	command("rm -r -f " + pattern);
}

/**
 * Validate a file existence.
 */
void validateFile(fs::path file, string msj) {
	if (!fs::is_regular_file(file)) {
		cerr << msj << " file " << file << " not found" << endl;
		exitWithError();
	}
}

/**
 * Files and Directory's validation
 */
void testingDirCheck(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//  Directory validation
	//--------------------------------------------------------------
	if (argc != 2) {
		cerr << "First param must be a testing directory." << endl;
		exitWithError();
	}

	fs::path testDir = argv[1];

	validateDir(testDir, "Test");
	validateDir(testDir / "examples", "Examples");
	validateDir(testDir / "logs", "Logs");

	fs::path simpleTest = testDir / "examples" / "simpleTest";
	validateDir(simpleTest / "input-logos", "Extractor input logos (examples)");
	validateDir(simpleTest / "output-logos",
			"Extractor output Logos (examples)");

	//--------------------------------------------------------------
	//  Files validation
	//--------------------------------------------------------------

	validateFile(testDir / "Anakin", "Anakin bin");
	validateFile(simpleTest / "several.jpg", "Several logos .jpg");

	// ====== SQL script File =======

	fs::path sqlScriptPath = testDir / "script.sql";
	validateFile(sqlScriptPath, "SQL script");

	string scriptContent = get_file_contents(sqlScriptPath.c_str());
	std::size_t found = scriptContent.find("DROP DATABASE IF EXISTS");
	if (found != std::string::npos) {
		cerr
				<< "Remove \"DROP DATABASE IF EXISTS ...\" and \"CREATE DATABASE ...\" from "
				<< sqlScriptPath << endl;
		exitWithError();
	}

}

/**
 * Setup new temporary environment variables for postgresql database.
 */
void setTestingEnvironmentVariables(string host, string database, string user,
		string password) {

#ifdef TESTING_DEBUGMODE
	cout << "--------  Pre-Environment setup  --------" << endl;
	cout << "PGHOST\t\t" << getenv("PGHOST") << endl;
	cout << "PGDATABASE\t" << getenv("PGDATABASE") << endl;
	cout << "PGUSER\t\t" << getenv("PGUSER") << endl;
	cout << "PGPASSWORD\t" << getenv("PGPASSWORD") << endl << endl;
#endif
	setenv("PGHOST", host.c_str(), 1);
	setenv("PGDATABASE", database.c_str(), 1);
	setenv("PGUSER", user.c_str(), 1);
	setenv("PGPASSWORD", password.c_str(), 1);
#ifdef TESTING_DEBUGMODE
	cout << "--------  Post-Environment setup  --------" << endl;
	cout << "PGHOST\t\t" << getenv("PGHOST") << endl;
	cout << "PGDATABASE\t" << getenv("PGDATABASE") << endl;
	cout << "PGUSER\t\t" << getenv("PGUSER") << endl;
	cout << "PGPASSWORD\t" << getenv("PGPASSWORD") << endl << endl;
#endif
}

/**
 * adapt a path to anakin compatible directory style
 */
string pathToAnakinDir(fs::path path) {
	string out = path.string();
	if (out.back() != '/') {
		return out + "/";
	}
	return out;
}

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin.
 */
void simpleTest(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//  Test Setup
	//--------------------------------------------------------------

	//testing database data
	string database = "AnakinAcceptanceTesting";
	string userDB = "postgres";
	string hostDB = "localhost";
	string passDB = "postgres";

	fs::path testDir = argv[1];
	fs::path anakinPath = testDir / "Anakin";
	fs::path sqlScriptPath = testDir / "script.sql";
	fs::path examplesDir = testDir / "examples";
	fs::path simpleTest = examplesDir / "simpleTest";
	fs::path inputLogos = simpleTest / "input-logos";
	fs::path outputLogos = simpleTest / "output-logos";
	fs::path severalJPG = simpleTest / "several.jpg";
	fs::path severalXML = simpleTest / "several.xml";
	fs::path logsDir = testDir / "logs";
	fs::path lastStderr = logsDir / "lastStderr.txt";
	fs::path lastStdout = logsDir / "lastStdout.txt";

	//testing database cleanup
	command("dropdb --if-exists " + database);

	//testing database creation.
	command("createdb " + database);

	//run SQL script into database.
	command(
			"psql -U " + userDB + " -d " + database + " -q -f "
					+ sqlScriptPath.string());

	//dir cleanups
	dirCleanup(outputLogos);
	dirCleanup(logsDir);
	command("rm -f " + severalXML.string());

	//setting up new testing temporary environment variables
	setTestingEnvironmentVariables(hostDB, database, userDB, passDB);

	//--------------------------------------------------------------
	//  Extractor Basic Test
	//--------------------------------------------------------------

	command(
			anakinPath.string() + " -modeextractor -matching -iFolder "
					+ pathToAnakinDir(inputLogos) + " -oPath "
					+ pathToAnakinDir(outputLogos) + " -lod -xml > "
					+ lastStdout.string() + " 2> " + lastStderr.string(), true);
	command(
			anakinPath.string() + " -modeextractor -matching -iFile "
					+ severalJPG.string() + " -oPath "
					+ pathToAnakinDir(simpleTest) + " -lod -xml > "
					+ lastStdout.string() + " 2> " + lastStderr.string(), true);

	//--------------------------------------------------------------
	//  Trainer Basic Test
	//--------------------------------------------------------------

//	command(
//			anakinPath.string() + " -modetrainer -user 1 -saveToFile "
//					+ severalXML.string() + " > " + lastStdout.string() + " 2> "
//					+ lastStderr.string(), true);
//
//	command(
//			anakinPath.string() + " -modetrainer -user 1 -path "
//					+ pathToAnakinDir(outputLogos) + " -patterns > "
//					+ lastStdout.string() + " 2> " + lastStderr.string(), true);
	//TODO verify stdout!


}

int main(int argc, const char * argv[]) {

testingDirCheck(argc, argv);

simpleTest(argc, argv);

exitWithSucces();
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

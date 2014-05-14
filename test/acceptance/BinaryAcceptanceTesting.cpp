/*
 * BinaryAcceptanceTesting.cpp
 *
 * For a more detailed output use #define TESTING_DEBUGMODE
 *
 *  Created on: 06/05/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING
#define TESTING_DEBUGMODE

#include <string>
#include <iostream>
#include <stdlib.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <utils/Files.hpp>
#include <signal.h>
#include <utils/Files.hpp>
#include <regex>

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
 * kill the process with pid = childPIDtoKill, and pid>0
 */
void killPID(pid_t childPIDtoKill) {
	if (childPIDtoKill > 0) {
		if (kill(childPIDtoKill, SIGTERM) == -1) {
			cerr << "pid " << childPIDtoKill << " can't be killed" << endl;
		}
#ifdef TESTING_DEBUGMODE
		else {
			cout << "pid " << childPIDtoKill << " killed" << endl;
		}
#endif
	}
}

/**
 * adapt a path to anakin compatible directory style
 */
string pathToAnakinPath(fs::path path) {
	string out = path.string();
	if (fs::is_directory(path)) {
		if (out.back() != '/') {
			out = out + "/";
		}
	}
	return "\"" + out + "\"";
}

/**
 * Execute a system call to a specific program. If vforkExit is true, _exit() will be
 * used (suitable for fork()) instead of exit(). If childPIDtoKill > 0
 * the child PID will be killed by a signal before exit;
 */
void command(string command, bool showLogMsjIfFail = false, bool forkExit =
		false, pid_t childPIDtoKill = 0) {
#ifdef TESTING_DEBUGMODE
	cout << "Command \"" << command << "\" executed" << endl;
#endif
	if (system(command.c_str()) != 0) {
		cerr << "Command \"" << command << "\" fail" << endl;
		if (showLogMsjIfFail) {
			cerr << "See log directory for program error outputs" << endl;
		}
		killPID(childPIDtoKill);
		if (forkExit) {
			_exit(EXIT_FAILURE);
		} else {
			exitWithError();
		}
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
	validateDir(simpleTest / "input-logos",
			"Extractor input logos (for examples)");
	validateDir(simpleTest / "output-logos",
			"Extractor output Logos (for examples)");
	validateDir(simpleTest / "outputs", "Program outputs (from examples)");

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

void stopAnakinHTTP(pid_t pID, fs::path logsDir) {
	command(
			"time curl -X POST -H \"Content-Type: application/json\" -d '{\"action\":\"stop\"}' --connect-timeout 10  -lv http://127.0.0.1:8080/ > "
					+ pathToAnakinPath((logsDir / "stopAnakinStdoutHTTP"))
					+ " 2> "
					+ pathToAnakinPath((logsDir / "stopAnakinStderrHTTP")),
			true, false, pID);
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
	fs::path outputs = simpleTest / "outputs";
	fs::path severalJPG = simpleTest / "several.jpg";
	fs::path severalXML = outputs / "several.xml";
	fs::path logsDir = testDir / "logs";
	fs::path lastStderr = logsDir / "lastStderr.txt";
	fs::path lastStdout = logsDir / "lastStdout.txt";
	fs::path patternMatchingLastStderr = logsDir
			/ "patternMatchingLastStderr.txt";
	fs::path patternMatchingLastStdout = logsDir
			/ "patternMatchingLastStdout.txt";
	fs::path trainerOutput = outputs / "trainerOutput";

	fs::path logsAnakin = logsDir / "anakin.log";
	fs::path logsDbConnector = logsDir / "dbconnector.log";
	fs::path logsTrainer = logsDir / "trainer.log";
	fs::path logsExtractor = logsDir / "extractor.log";

	//testing database cleanup
	command("dropdb --if-exists " + database);

	//testing database creation.
	command("createdb " + database);

	//run SQL script into database.
	command(
			"psql -U " + userDB + " -d " + database + " -q -f "
					+ pathToAnakinPath(sqlScriptPath));

	//dir cleanups
	dirCleanup(outputLogos);
	dirCleanup(logsDir);
	dirCleanup(outputs);
	command("rm -f " + pathToAnakinPath(severalXML));

	//setting up new testing temporary environment variables
	setTestingEnvironmentVariables(hostDB, database, userDB, passDB);

	//TODO verify stdout!

	//--------------------------------------------------------------
	//  Step 1 - Extractor Basic Test
	//--------------------------------------------------------------

	command(
			pathToAnakinPath(anakinPath) + " -modeextractor -oLogFile "
					+ pathToAnakinPath(logsExtractor) + " -matching -iFolder "
					+ pathToAnakinPath(inputLogos) + " -oPath "
					+ pathToAnakinPath(outputLogos) + " -lod -xml > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);
	command(
			pathToAnakinPath(anakinPath) + " -modeextractor -oLogFile "
					+ pathToAnakinPath(logsExtractor) + " -matching -iFile "
					+ pathToAnakinPath(severalJPG) + " -oPath "
					+ pathToAnakinPath(outputs) + " -lod -xml > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);
	//--------------------------------------------------------------
	//  Step 2 - DBconnector Basic Test
	//--------------------------------------------------------------

	command(
			pathToAnakinPath(anakinPath) + " -modedbconnector -oLogFile "
					+ pathToAnakinPath(logsDbConnector) + " -scenes -path "
					+ pathToAnakinPath(severalXML) + " > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);

	command(
			pathToAnakinPath(anakinPath) + " -modedbconnector -oLogFile "
					+ pathToAnakinPath(logsDbConnector) + " -user 1 -path "
					+ pathToAnakinPath(outputLogos) + " -patterns > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);

	//--------------------------------------------------------------
	//  Step 3 - Trainer Basic Test
	//--------------------------------------------------------------

	command(
			pathToAnakinPath(anakinPath) + " -modetrainer -oLogFile "
					+ pathToAnakinPath(logsTrainer) + " -user 1 -saveToFile "
					+ pathToAnakinPath(trainerOutput) + " > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);

	//--------------------------------------------------------------
	//  Step 4 - DBconnector Basic Test (continue)
	//--------------------------------------------------------------

	command(
			pathToAnakinPath(anakinPath) + " -modedbconnector -oLogFile "
					+ pathToAnakinPath(logsDbConnector) + " -index "
					+ pathToAnakinPath(trainerOutput) + " 1 -savePatterns > "
					+ pathToAnakinPath(lastStdout) + " 2> "
					+ pathToAnakinPath(lastStderr), true);

	//--------------------------------------------------------------
	//  Step 5 - PatternMatching Basic Test
	//--------------------------------------------------------------

	pid_t pID = fork();
	if (pID == 0) { // child
		// Code only executed by child process

		command(
				pathToAnakinPath(anakinPath)
						+ " -modepatternmatching -oLogFile "
						+ pathToAnakinPath(logsAnakin)
						+ " -iHTTP 8080 -oHTTP -verbose > "
						+ pathToAnakinPath(patternMatchingLastStdout) + " 2> "
						+ pathToAnakinPath(patternMatchingLastStderr), true,
				true);
		_exit(EXIT_SUCCESS);
	} else if (pID < 0) { // failed to fork
		cerr << "Step 5 - Failed to fork for PatternMatching" << endl;
		exitWithError();
	} else { // parent
		// Code only executed by parent process
		sleep(2);
		//repeated 3 times, to obtain solid outputs
		for (int i = 0; i < 3; i++) {
			command(
					"time curl -X POST -H \"Content-Type: application/json\" -d '{\"indexes\":[1], \"action\":\"matching\", \"scenario\":1}' --connect-timeout 10  -lv http://127.0.0.1:8080/ > "
							+ pathToAnakinPath(lastStdout) + " 2> "
							+ pathToAnakinPath(lastStderr), true, false, pID);

			//Analyzing output
			string pattern = "{\"category\":\"PATTERN\",\"requestID\":\"";
			std::string capture = get_file_contents(lastStdout.c_str());
			if (capture.find(pattern) == std::string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< "\n\n" << capture << "\n\n"
						<< "and should replied something that start with:"
						<< "\n\n" << pattern << endl;
				stopAnakinHTTP(pID, logsDir);
				exitWithError();
			}

			pattern =
					"\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}]}";
			if (capture.find(pattern) == std::string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< "\n\n" << capture << "\n\n"
						<< "and should replied something that end with:"
						<< "\n\n" << pattern << endl;
				stopAnakinHTTP(pID, logsDir);
				exitWithError();
			}
		}
		stopAnakinHTTP(pID, logsDir);
	}
}

int main(int argc, const char * argv[]) {

	testingDirCheck(argc, argv);

	simpleTest(argc, argv);

	exitWithSucces();
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

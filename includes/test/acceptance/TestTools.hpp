/*
 * tools.hpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef TESTTOOLS_HPP_
#define TESTTOOLS_HPP_

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <boost/chrono/duration.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/timer/timer.hpp>
#include <connection/Daemon.hpp>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/Files.hpp>
#include <utils/statistics/StatisticsCollector.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace Anakin;
namespace fs = boost::filesystem;

/**
 * Separate a string into tokens, ignoring quoted phrases.
 */
void splitTokens(const string& str, vector<string>& outputVector);

/**
 * used to fail the test
 */
void exitWithError();

/**
 * used to succeed the test
 */
void exitWithSucces();

/**
 * Msj used to start a new test
 */
void printTestMsj(string msj);

/**
 *  Print final statistics
 */
void printStatistics(StatisticsCollector* collector);

/**
 * kill the process with pid = childPIDtoKill, and pid>0
 */
void killPID(pid_t childPIDtoKill);

/**
 * adapt a path to anakin compatible directory style
 */
string pathToAnakinPath(fs::path path);

/**
 * Execute a system call to a specific program. If vforkExit is true, _exit() will be
 * used (suitable for fork()) instead of exit(). If childPIDtoKill > 0
 * the child PID will be killed by a signal before exit;
 */
void command(StatisticsCollector* collector, bool verbose, string command,
		bool showLogMsjIfFail = false, bool forkExit = false,
		pid_t childPIDtoKill = 0);

/**
 * Validate a directory existence.
 */
void validateDir(fs::path path, string msj);

/**
 * Delete all dir content, except for the directory itself.
 */
void dirCleanup(fs::path path);

/**
 * Validate a file existence.
 */
void validateFile(fs::path file, string msj);

/**
 * Files and Directory's validation
 */
void testingDirCheck(int argc, const char * argv[]);

/**
 * Setup new temporary environment variables for postgresql database.
 */
void setTestingEnvironmentVariables(string host, string database, string user,
		string password);

void stopAnakinHTTP(pid_t pID, fs::path logsDir,
		StatisticsCollector* collector);

void printStep(string test, int number);

/**
 * Run a simple program with the given commands
 */
template<class SpecificSimpleProgram>
void runProgram(StatisticsCollector* collector, string currentCommand) {
	Program* program = new SpecificSimpleProgram();
	cout
			<< "______________________________________________________________________"
			<< endl << "* Program: " << program->getProgramName() << endl
			<< "* Command \"" << currentCommand << "\" executed" << endl
			<< "* Output:" << endl << endl;
	vector<string> input(0);
	splitTokens(currentCommand, input);

	boost::timer::cpu_timer timer;
	int signal = program->start(&input);
	auto nanoseconds = boost::chrono::nanoseconds(
			timer.elapsed().user + timer.elapsed().system);
	boost::chrono::milliseconds milliseconds = boost::chrono::duration_cast<
			boost::chrono::milliseconds>(nanoseconds);

	cout << endl << "* Elapsed Time: " << milliseconds.count() << " ms."
			<< endl;
	collector->addItem(program->getProgramName() + " " + currentCommand,
			milliseconds.count());
	delete program;
	if (signal == EXIT_FAILURE) {
		exitWithError();
	}
}

/**
 * Run a simple program with the given commands
 */
template<class SpecificDaemon>
void runDaemonProgram(StatisticsCollector* collector, string currentCommand) {
	SpecificDaemon commandRunner;
	Program* program = new Daemon<SpecificDaemon>();
	cout
			<< "______________________________________________________________________"
			<< endl << "* Daemon Program: " << commandRunner.getProgramName()
			<< endl << "* Command \"" << currentCommand << "\" executed" << endl
			<< "* Output:" << endl << endl;
	vector<string> input(0);
	splitTokens(currentCommand, input);

	boost::timer::cpu_timer timer;
	int signal = program->start(&input);
	auto nanoseconds = boost::chrono::nanoseconds(
			timer.elapsed().user + timer.elapsed().system);
	boost::chrono::milliseconds milliseconds = boost::chrono::duration_cast<
			boost::chrono::milliseconds>(nanoseconds);

	collector->addItem(program->getProgramName() + " " + currentCommand,
			milliseconds.count());

	if (signal == EXIT_FAILURE) {
		exitWithError();
	}
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* TESTTOOLS_HPP_ */

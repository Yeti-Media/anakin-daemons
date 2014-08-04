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
#include <chrono>
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
#include <pthread.h>

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
void printTestMsj(string msj, uint testRepetition);

/**
 *  Print final statistics
 */
void printStatistics(StatisticsCollector* collector);

/**
 * kill the process with pid = childPIDtoKill, and pid>0
 */
void killThread(pthread_t * threadToKill);

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
		bool showLogMsjIfFail = false);

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

/**
 * Stop anakin. If cllector is NULL, no statistics will be collected.
 */
void stopAnakinHTTP(pthread_t * thread, fs::path logsDir,
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

	auto begin = chrono::high_resolution_clock::now();
	int signal = program->start(&input);
	if (collector != NULL) {
		auto delay = chrono::high_resolution_clock::now() - begin;
		double ms = (double) std::chrono::duration_cast<
				std::chrono::milliseconds>(delay).count();

		cout << endl << "* Elapsed Time: " << ms << " ms." << endl;
		collector->addItem(program->getProgramName() + " " + currentCommand,
				ms);
	}
	delete program;
	if (signal == EXIT_FAILURE) {
		exitWithError();
	}
}
//
///**
// * Run a simple program with the given commands
// */
//template<class SpecificDaemon>
//void runDaemonProgram(StatisticsCollector* collector, string currentCommand) {
//	SpecificDaemon commandRunner;
//	Program* program = new Daemon<SpecificDaemon>();
//	cout
//			<< "______________________________________________________________________"
//			<< endl << "* Daemon Program: " << commandRunner.getProgramName()
//			<< endl << "* Command \"" << currentCommand << "\" executed" << endl
//			<< "* Output:" << endl << endl;
//	vector<string> input(0);
//	splitTokens(currentCommand, input);
//
//	auto begin = chrono::high_resolution_clock::now();
//	int signal = program->start(&input);
//	if (collector != NULL) {
//		auto delay = chrono::high_resolution_clock::now() - begin;
//		double ms = (double) std::chrono::duration_cast<
//				std::chrono::milliseconds>(delay).count();
//
//		cout << endl << "* Elapsed Time: " << ms << " ms." << endl;
//		collector->addItem(program->getProgramName() + " " + currentCommand,
//				ms);
//	}
//	delete program;
//
//	if (signal == EXIT_FAILURE) {
//		exitWithError();
//	}
//}

struct DaemonArgs {
	Program* program;
	vector<string> * input;
	DaemonArgs(Program* program, vector<string> * input) :
			program(program), input(input) {
	}
	~DaemonArgs() {
		delete program;
		delete input;
	}
};

void * startDaemon(void *ptr);

/**
 * Run a simple program with the given commands
 */
template<class SpecificDaemon>
pthread_t * runDaemonProgram(string currentCommand) {
	SpecificDaemon commandRunner;
	Program* program = new Daemon<SpecificDaemon>();
	cout
			<< "______________________________________________________________________"
			<< endl << "* Daemon Program: " << commandRunner.getProgramName()
			<< endl << "* Command \"" << currentCommand << "\" executed" << endl
			<< "* Output:" << endl << endl;
	vector<string> * input = new vector<string>(0);
	splitTokens(currentCommand, *input);

	pthread_t * thread = new pthread_t();

	DaemonArgs* dargs = new DaemonArgs(program, input);
	pthread_create(thread, NULL, startDaemon, (void*) dargs);
	return thread;
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* TESTTOOLS_HPP_ */

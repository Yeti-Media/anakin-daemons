/*
 * tools.hpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef TESTTOOLS_HPP_
#define TESTTOOLS_HPP_

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <curl/curl.h>
#include <curl/easy.h>
#include <boost/chrono/duration.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <chrono>
#include <connection/Daemon.hpp>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils/files/Files.hpp>
#include <utils/statistics/StatisticsCollector.hpp>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <utils/StringUtils.hpp>


using namespace std;
using namespace Anakin;
namespace fs = boost::filesystem;

struct curl_slist;

static inline size_t WriteCallback(void *contents, size_t size, size_t nmemb,
		void *userp) {
	((std::string*) userp)->append((char*) contents, size * nmemb);
	return size * nmemb;
}

static inline double cURL_JSON(const string & url, const string & data,
		string & output, string & error) {
	CURL *curl;
	CURLcode res;
	double time = 0;

	struct curl_slist *headers = NULL; // init to NULL is important

	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charsets: utf-8");

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
		curl_easy_setopt(curl, CURLOPT_POST, 1);

		res = curl_easy_perform(curl);

		if (CURLE_OK == res) {
			res = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &time);
			if (CURLE_OK == res) {
				time = time * 1000;
			}
		} else {
			error = string(curl_easy_strerror(res));
		}
	}
	curl_easy_cleanup(curl);
	return time;
}


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
void printStatistics(StatisticsCollector* collector, const string & file);

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
 * Return elapsed time in miliseconds (if vervose only)
 */
double command(StatisticsCollector* collector, bool verbose,
		const string & command, const string & group, bool showLogMsjIfFail =
				false);

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

void runCURL_JSON(StatisticsCollector* collector, const string & JSONcommand,
		const string & group, string & results);

/**
 * Run a simple program with the given commands
 */
template<class SpecificSimpleProgram>
void runProgram(StatisticsCollector* collector, const string & currentCommand,
		const string & group) {
	Program* program = new SpecificSimpleProgram();
	cout
			<< "______________________________________________________________________"
			<< endl << "* Program: " << program->getProgramName() << endl
			<< "* Command \"" << currentCommand << "\" executed" << endl
			<< "* Output:" << endl << endl;
	Ptr<vector<string>> input = makePtr<vector<string>>();
	stringutils::tokenizeWordsIgnoringQuoted(currentCommand, *input);

	auto begin = chrono::high_resolution_clock::now();
	int signal = program->start(input);
	if (collector != NULL) {
		auto delay = chrono::high_resolution_clock::now() - begin;
		double ms = (double) std::chrono::duration_cast<
				std::chrono::milliseconds>(delay).count();

		cout << endl << "* Elapsed Time: " << ms << " ms." << endl;
		collector->addItem(program->getProgramName() + " " + currentCommand,
				group, ms);
	}
	delete program;
	if (signal == EXIT_FAILURE) {
		exitWithError();
	}
}

struct DaemonArgs {
	Ptr<Program> program;
	Ptr<vector<string>> input;
	DaemonArgs(Program* program, Ptr<vector<string>> input) :
			program(program), input(input) {
	}
	~DaemonArgs() {
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
	Ptr<vector<string>> input = makePtr<vector<string>>();
	stringutils::tokenizeWordsIgnoringQuoted(currentCommand, *input);
	pthread_t * thread = new pthread_t();

	DaemonArgs* dargs = new DaemonArgs(program, input);
	pthread_create(thread, NULL, startDaemon, (void*) dargs);
	return thread;
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

#endif /* TESTTOOLS_HPP_ */

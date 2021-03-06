/*
 * TestTools.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <test/acceptance/TestTools.hpp>

using namespace std;
using namespace Anakin;
namespace fs = boost::filesystem;

void runCURL_JSON(StatisticsCollector* collector, const string & JSONcommand,
		const string & group, string & results) {
	cout
			<< "______________________________________________________________________"
			<< endl << "* Program: cURL with JSON" << endl << "* JSON Command: "
			<< JSONcommand << endl << "* Output:" << endl << endl;
	string url = "http://127.0.0.1:8080/";
	string error;
	double time;

	time = cURL_JSON(url, JSONcommand, results, error);
	cout << results << endl << "* Elapsed Time: " << time << " ms." << endl;
	if (collector != NULL) {
		collector->addItem(JSONcommand, group, time);
	}
	if (!error.empty()) {
		cerr << "cURL fail reason: " << error << endl;
		exitWithError();
	}
}

void * startDaemon(void *ptr) {
	DaemonArgs* dargs = (DaemonArgs*) ptr;
	int signal = dargs->program->start(dargs->input);
	delete dargs;
	if (signal == EXIT_FAILURE) {
		exitWithError();
	}
}

/**
 * used to fail the test
 */
void exitWithError() {
	cerr << "\n===============================================\n"
			<< "* Acceptance test result: FAIL" << endl;
	exit(EXIT_FAILURE);
}

/**
 * used to succeed the test
 */
void exitWithSucces() {
	cout << "\n===============================================\n"
			<< "* Acceptance test result: SUCCESS" << endl;
	exit(EXIT_SUCCESS);
}

/**
 *  Print final statistics
 */
void printStatistics(StatisticsCollector* collector, const string & file) {
	cout << endl
			<< "======================================================================\n"
			<< "* Benchmark Results printed on file: " << file << endl;
	write_to_file(collector->compute(), file);

//	cout << endl << "===============================================\n"
//	<< "************  Benchmark Results  **************\n"
//	<< "===============================================\n"
//	<< collector->compute();
}

/**
 *  Print final statistics
 */
void printTestMsj(string msj, uint testRepetition) {
	cout << endl
			<< "======================================================================"
			<< endl << "***** Testing: " << msj << endl
			<< "***** Repetition number " << testRepetition << endl
			<< "======================================================================"
			<< endl;
}

/**
 * adapt a path to anakin compatible directory style
 */
string pathToAnakinPath(fs::path path) {
	string out = path.string();
	if (fs::is_directory(path)) {
		if (out.back() != '/') {
			out.append("/");
		}
	}
	return "\"" + out + "\"";
}

/**
 * Execute a system call to a specific program. If vforkExit is true, _exit() will be
 * used (suitable for fork()) instead of exit(). If childPIDtoKill > 0
 * the child PID will be killed by a signal before exit;
 */
double command(StatisticsCollector* collector, bool verbose,
		const string & command, const string & group, bool showLogMsjIfFail) {
	double elpasedTime = 0;
	if (verbose) {
		cout
				<< "______________________________________________________________________"
				<< endl << "* Command \"" << command << "\" executed" << endl
				<< "* Output:" << endl << endl;
	}
	auto begin = chrono::high_resolution_clock::now();
	if (system(command.c_str()) != 0) {
		cerr << "Command \"" << command << "\" fail" << endl;
		if (showLogMsjIfFail) {
			cerr << "See log directory for program error outputs" << endl;
		}
		exitWithError();
	} else {
		auto delay = chrono::high_resolution_clock::now() - begin;
		elpasedTime = (double) std::chrono::duration_cast<
				std::chrono::milliseconds>(delay).count();
		if (verbose) {
			cout << endl << "* Elapsed Time: " << elpasedTime << " ms." << endl;
			if (collector != NULL) {
				collector->addItem(command, group, elpasedTime);
			}
		}
	}
	return elpasedTime;
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
	string pattern = "\"" + path.string() + "/\"*";
	command(NULL, false, "rm -r -f " + pattern, "cleanup");
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

	fs::path simpleTest = testDir / "examples" / "simpleTest";
	validateDir(simpleTest / "input-logos",
			"Extractor input logos (for examples)");

	fs::path OCRDemoBenchmark = testDir / "examples" / "ocr";
	validateDir(OCRDemoBenchmark / "dataset_images", "OCR Image dataset");
	validateDir(OCRDemoBenchmark / "text_localization",
			"OCR Image dataset text localization");
	validateDir(OCRDemoBenchmark / "word_recognition",
			"OCR Image dataset word recognition");

	fs::path classifier = testDir / "examples" / "ocr" / "classifier";

	validateDir(classifier, "OCR Classifiers dir");

	//--------------------------------------------------------------
	//  Files validation
	//--------------------------------------------------------------

	validateFile(simpleTest / "several.jpg", "Several logos .jpg");
	validateFile(classifier / "trained_classifierNM1.xml",
			"Classifier trained_classifierNM1");
	validateFile(classifier / "trained_classifierNM2.xml",
			"Classifier trained_classifierNM2");
	validateFile(classifier / "OCRHMM_transitions_table.xml",
			"Classifier OCRHMM_transitions_table");
	validateFile(classifier / "OCRHMM_knn_model_data.xml.gz",
			"Classifier OCRHMM_knn_model_data.xml.gz");
	validateFile(classifier / "trained_classifier_erGrouping.xml",
			"Classifier trained_classifier_erGrouping.xml");

	// ====== SQL script File =======

	fs::path sqlScriptPath = testDir / "script.sql";
	validateFile(sqlScriptPath, "SQL script");

	Ptr<string> scriptContent = get_file_contents(sqlScriptPath.string());
	std::size_t found = scriptContent->find("DROP DATABASE IF EXISTS");
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

void stopAnakinHTTP(pthread_t * thread, fs::path logsDir,
		StatisticsCollector* collector) {
	string results;
	string JSONcmd = "{\"action\":\"stop\"}";
	runCURL_JSON(collector, JSONcmd, "ServerStop", results);
	pthread_join(*thread, NULL);
	delete thread;
}

void printStep(string test, int number) {
	cout << endl
			<< "----------------------------------------------------------------------"
			<< endl << "* Testing: " << test << " - Step " << number << endl
			<< "----------------------------------------------------------------------"
			<< endl;
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/


/*
 * benchmarkTest.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <test/acceptance/TestTools.hpp>
#include <test/acceptance/TestDefinitions.hpp>
#include <boost/filesystem/path.hpp>
#include <processing/commandrunner/PatternMatcher.hpp>
#include <processing/simpleprogram/PatternDBConnector.hpp>
#include <processing/simpleprogram/PatternExtractor.hpp>
#include <processing/simpleprogram/PatternTrainer.hpp>
#include <unistd.h>
#include <utils/Files.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin, but with heavy examples, to test performance.
 */
void benchmarkTest(int argc, const char * argv[],
		StatisticsCollector* collector) {

	//--------------------------------------------------------------
	//  Test Setup
	//--------------------------------------------------------------
	string testName = "Benchmark";
	uint maxTestRepetition = 10;

	string database = "AnakinBenckmarkTesting";
	string userDB = "postgres";
	string hostDB = "localhost";
	string passDB = "postgres";

	fs::path testDir = argv[1];
	fs::path sqlScriptPath = testDir / "script.sql";
	fs::path examplesDir = testDir / "examples";
	fs::path benchmark = examplesDir / "benchmark";
	fs::path inputLogos = benchmark / "input-logos";
	fs::path outputLogos = benchmark / "output-logos";
	fs::path outputs = benchmark / "outputs";
	fs::path severalJPG = benchmark / "several.jpg";
	fs::path severalXML = outputs / "several.xml";
	fs::path logsDir = testDir / "logs";
	fs::path lastStderr = logsDir / "lastStderr.txt";
	fs::path lastStdout = logsDir / "lastStdout.txt";
	fs::path trainerOutput = outputs / "patternTrainerOutput";

	fs::path logsAnakin = logsDir / "patternMatcher.log";
	fs::path logsDbConnector = logsDir / "patternDBconnector.log";
	fs::path logsTrainer = logsDir / "patternTrainer.log";
	fs::path logsExtractor = logsDir / "patternExtractor.log";

	for (uint testRepetition = 1; testRepetition <= maxTestRepetition;
			testRepetition++) {
		printTestMsj(testName, testRepetition);

		//testing database cleanup
		command(NULL, false, "dropdb --if-exists " + database);

		//testing database creation.
		command(NULL, false, "createdb " + database);

		//run SQL script into database.
		command(NULL, false,
				"psql -U " + userDB + " -d " + database + " -q -f "
						+ pathToAnakinPath(sqlScriptPath));

		//dir cleanups
		dirCleanup(outputLogos);
		dirCleanup(logsDir);
		dirCleanup(outputs);
		command(NULL, false, "rm -f " + pathToAnakinPath(severalXML));

		//setting up new testing temporary environment variables
		setTestingEnvironmentVariables(hostDB, database, userDB, passDB);

		//TODO verify stdout!
		//--------------------------------------------------------------
		//  Step 1 - Extractor Basic Test
		//--------------------------------------------------------------
		printStep(testName, 1);

		runProgram<PatternExtractor>(collector,
				"-oLogFile " + pathToAnakinPath(logsExtractor)
						+ " -matching -iFolder " + pathToAnakinPath(inputLogos)
						+ " -oPath " + pathToAnakinPath(outputLogos)
						+ " -lod -xml");

		runProgram<PatternExtractor>(collector,
				"-oLogFile " + pathToAnakinPath(logsExtractor)
						+ " -matching -iFile " + pathToAnakinPath(severalJPG)
						+ " -oPath " + pathToAnakinPath(outputs)
						+ " -lod -xml");

		//--------------------------------------------------------------
		//  Step 2 - DBconnector Basic Test
		//--------------------------------------------------------------
		printStep(testName, 2);

		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector)
						+ " -scenes -path " + pathToAnakinPath(severalXML));

		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector)
						+ " -user 1 -path " + pathToAnakinPath(outputLogos)
						+ " -patterns");

		//--------------------------------------------------------------
		//  Step 3 - Trainer Basic Test
		//--------------------------------------------------------------
		printStep(testName, 3);
		runProgram<PatternTrainer>(collector,
				"-oLogFile " + pathToAnakinPath(logsTrainer)
						+ " -user 1 -saveToFile "
						+ pathToAnakinPath(trainerOutput));
		//--------------------------------------------------------------
		//  Step 4 - DBconnector Basic Test (continue)
		//--------------------------------------------------------------
		printStep(testName, 4);
		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector) + " -index "
						+ pathToAnakinPath(trainerOutput) + " 1 -savePatterns");

		//--------------------------------------------------------------
		//  Step 5 - PatternMatching Basic Test
		//--------------------------------------------------------------
		printStep(testName, 5);

		pthread_t * thread = NULL;
		thread = runDaemonProgram<PatternMatcher>(
				"-oLogFile " + pathToAnakinPath(logsAnakin)
						+ " -iHTTP 8080 -oHTTP -verbose");

		//check if the server start
		bool serverStarted = false;
		while (!serverStarted) {
			sleep(2);
			std::string * capture = get_file_contents(logsAnakin.c_str());
			if (capture->find("* Server started *") != std::string::npos) {
				serverStarted = true;
			}
		}

		//repeated "query" times, to obtain solid benchmarks
		for (int query = 1; query <= 10; query++) {

			command(collector, true,
					"time curl -X POST -H \"Content-Type: application/json\" -d '{\"indexes\":[1], \"action\":\"matching\", \"scenario\":1}' --connect-timeout 10  -lv http://127.0.0.1:8080/ > "
							+ pathToAnakinPath(lastStdout) + " 2> "
							+ pathToAnakinPath(lastStderr), true);
			cout << "* Request number " << query << endl;
			//Analyzing output
			string pattern = "{\"category\":\"PATTERN\",\"requestID\":\"";
			std::string * capture = get_file_contents(lastStdout.c_str());
			if (capture->find(pattern) == std::string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< endl << endl << capture << endl << endl
						<< "and should replied something that start with:"
						<< endl << endl << pattern << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}
			pattern =
					"\",\"values\":[{\"center\":{\"x\":202.592300415039,\"y\":361.972229003906},\"label\":\"3\"}]}]}";
			if (capture->find(pattern) == std::string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< endl << endl << capture << endl << endl
						<< "and should replied something that end with:" << endl
						<< endl << pattern << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}
			delete capture;
		}
		stopAnakinHTTP(thread, logsDir, collector);

		//Closing remaining connections
		cout << endl
				<< "******* Closing remaining connections (ignore warning) *********"
				<< endl;
		command(NULL, false,
				"psql -U " + userDB + " -d " + database + " -q -c "
						+ "\"SELECT pg_terminate_backend(pg_stat_activity.pid) FROM pg_stat_activity WHERE pg_stat_activity.datname = \'"
						+ database + "\' AND pid <> pg_backend_pid();\"");
	} // testRepetition cicle
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

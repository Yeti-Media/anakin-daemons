/*
 * spatternSimpleTest.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */
#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <test/acceptance/TestTools.hpp>
#include <test/acceptance/TestDefinitions.hpp>
#include <boost/filesystem/path.hpp>
#include <processing/commandrunner/PatternMatcher.hpp>
#include <processing/simpleprogram/PatternDBConnector.hpp>
#include <processing/simpleprogram/PatternExtractor.hpp>
#include <processing/simpleprogram/PatternTrainer.hpp>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>

/**
 * This is a very simple test used as an example, and for quickly test all the
 * programs and subprograms from Anakin.
 */
void patternSimpleTest(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//  Test Setup
	//--------------------------------------------------------------

	string testName = "Pattern_Simple";
	uint maxTestRepetition = 10;

	string database = "AnakinAcceptanceTesting";
	string userDB = "postgres";
	string hostDB = "localhost";
	string passDB = "postgres";

	fs::path ramDir("/tmp/ram/Anakin/AcceptanceTests/" + testName);
	if (!fs::is_directory(ramDir)) {
		fs::create_directories(ramDir);
	}

	fs::path testDir = argv[1];
	fs::path sqlScriptPath = testDir / "script.sql";
	fs::path examplesDir = testDir / "examples";
	fs::path simpleTest = examplesDir / "simpleTest";
	fs::path inputLogos = simpleTest / "input-logos";
	fs::path outputLogos = ramDir / "output-logos";
	if (!fs::is_directory(outputLogos)) {
		fs::create_directories(outputLogos);
	}
	fs::path outputs = ramDir / "outputs";
	if (!fs::is_directory(outputs)) {
		fs::create_directories(outputs);
	}
	fs::path severalJPG = simpleTest / "several.jpg";
	fs::path severalXML = outputs / "several.xml";
	fs::path dpizzaPNG = simpleTest / "dpizza.png";
	fs::path dpizzaXML = outputs / "dpizza.xml";
	fs::path logsDir = ramDir / "logs";
	if (!fs::is_directory(logsDir)) {
		fs::create_directories(logsDir);
	}
	fs::path lastStderr = logsDir / "lastStderr.txt";
	fs::path lastStdout = logsDir / "lastStdout.txt";
	fs::path trainerOutput = outputs / "patternTrainerOutput";

	fs::path logsAnakin = logsDir / "patternMatcher.log";
	fs::path logsDbConnector = logsDir / "patternDBconnector.log";
	fs::path logsTrainer = logsDir / "patternTrainer.log";
	fs::path logsExtractor = logsDir / "patternExtractor.log";
	fs::path benchmarkResults = ramDir / "benchmarkResults.txt";

	StatisticsCollector * collector = new StatisticsCollector();

	for (uint testRepetition = 1; testRepetition <= maxTestRepetition;
			testRepetition++) {
		printTestMsj(testName, testRepetition);

		//testing database cleanup
		command(NULL, false, "dropdb --if-exists " + database, "cleanup");

		//testing database creation.
		command(NULL, false, "createdb " + database, "cleanup");

		//run SQL script into database.
		command(NULL, false,
				"psql -U " + userDB + " -d " + database + " -q -f "
						+ pathToAnakinPath(sqlScriptPath), "cleanup");

		//dir cleanups
		dirCleanup(outputLogos);
		dirCleanup(logsDir);
		dirCleanup(outputs);
		command(NULL, false, "rm -f " + pathToAnakinPath(severalXML),
				"cleanup");

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
						+ " -lod -xml", "PatternExtractor|XML");

		runProgram<PatternExtractor>(collector,
				"-oLogFile " + pathToAnakinPath(logsExtractor)
						+ " -matching -iFile " + pathToAnakinPath(severalJPG)
						+ " -oPath " + pathToAnakinPath(outputs) + " -lod -xml",
				"PatternExtractor|XML");

		runProgram<PatternExtractor>(collector,
				"-oLogFile " + pathToAnakinPath(logsExtractor)
						+ " -matching -iFile " + pathToAnakinPath(dpizzaPNG)
						+ " -oPath " + pathToAnakinPath(outputs) + " -lod -xml",
				"PatternExtractor|XML");

		//--------------------------------------------------------------
		//  Step 2 - DBconnector Basic Test
		//--------------------------------------------------------------
		printStep(testName, 2);

		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector)
						+ " -scenes -path " + pathToAnakinPath(severalXML),
				"PatternDBConnector|scenes");

		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector)
						+ " -scenes -path " + pathToAnakinPath(dpizzaXML),
				"PatternDBConnector|scenes");

		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector)
						+ " -user 1 -path " + pathToAnakinPath(outputLogos)
						+ " -patterns", "PatternDBConnector|patterns");

		//--------------------------------------------------------------
		//  Step 3 - Trainer Basic Test
		//--------------------------------------------------------------
		printStep(testName, 3);
		runProgram<PatternTrainer>(collector,
				"-oLogFile " + pathToAnakinPath(logsTrainer)
						+ " -user 1 -saveToFile "
						+ pathToAnakinPath(trainerOutput), "PatternTrainer");
		//--------------------------------------------------------------
		//  Step 4 - DBconnector Basic Test (continue)
		//--------------------------------------------------------------
		printStep(testName, 4);
		runProgram<PatternDBConnector>(collector,
				"-oLogFile " + pathToAnakinPath(logsDbConnector) + " -index "
						+ pathToAnakinPath(trainerOutput) + " 1 -savePatterns",
				"PatternDBConnector|savePatterns");

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
			Ptr<string> capture = get_file_contents(logsAnakin.c_str());
			if (capture->find("* Server started *") != string::npos) {
				serverStarted = true;
			}
		}

		string JSONcmd1 =
				"{\"indexes\":[1], \"action\":\"matching\", \"scenario\":1}";
		string JSONcmd2 =
				"{\"indexes\":[1], \"action\":\"matching\", \"scenario\":2}";

		//repeated "query" times, to obtain solid benchmarks
		for (int query = 1; query <= 10; query++) {
			string results;
			runCURL_JSON(collector, JSONcmd1, "PatternMatcher|Curl|matching",
					results);

			cout << "* Request number " << query << endl;

			//Analyzing output
			string pattern = "{\"category\":\"PATTERN\",\"requestID\":\"";
			if (results.find(pattern) == string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< endl << endl << results << endl << endl
						<< "and should replied something that start with:"
						<< endl << endl << pattern << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}

			pattern =
					"\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.527465820312,\"y\":151.533798217773},\"label\":\"9\"}]}]}";
			if (results.find(pattern) == string::npos) {
				// compare with a similar pattern.
				pattern =
						"\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":228.708847045898,\"y\":151.395462036133},\"label\":\"9\"}]}]}";
				if (results.find(pattern) == string::npos) {
					// compare with a similar pattern.
					pattern =
							"\",\"values\":[{\"label\":\"1\",\"values\":[{\"center\":{\"x\":100.817237854004,\"y\":68.1070556640625},\"label\":\"5\"},{\"center\":{\"x\":95.6366119384766,\"y\":231.299835205078},\"label\":\"8\"},{\"center\":{\"x\":229.288116455078,\"y\":151.411666870117},\"label\":\"9\"}]}]}";
					if (results.find(pattern) == string::npos) {
						cerr
								<< "PatternMatching subprogram wrong output. Anakin replied:"
								<< endl << endl << results << endl << endl
								<< "and should replied something that end with:"
								<< endl << endl << pattern << endl;
						stopAnakinHTTP(thread, logsDir, NULL);
						exitWithError();
					}

				}

			}

			//Trying other query

			runCURL_JSON(collector, JSONcmd2, "PatternMatcher|Curl|matching",
					results);

			cout << "* Request number " << query << endl;

			//Analyzing output
			string pattern2 = "{\"category\":\"PATTERN\",\"requestID\":\"";
			if (results.find(pattern2) == string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< endl << endl << results << endl << endl
						<< "and should replied something that start with:"
						<< endl << endl << pattern2 << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}

			pattern2 =
					"\",\"values\":[{\"label\":\"2\",\"values\":[{\"center\":{\"x\":133.31559753418,\"y\":146.899322509766},\"label\":\"7\"}]}]}";
			if (results.find(pattern2) == string::npos) {
				cerr
						<< "PatternMatching subprogram wrong output. Anakin replied:"
						<< endl << endl << results << endl << endl
						<< "and should replied something that end with:" << endl
						<< endl << pattern2 << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}
		}
		stopAnakinHTTP(thread, logsDir, collector);

		//Closing remaining connections
		cout << endl
				<< "******* Closing remaining connections (ignore warning) *********"
				<< endl;
		command(NULL, false,
				"psql -U " + userDB + " -d " + database + " -q -n -c "
						+ "\"SELECT pg_terminate_backend(pg_stat_activity.pid) FROM pg_stat_activity WHERE pg_stat_activity.datname = \'"
						+ database + "\' AND pid <> pg_backend_pid();\"",
				"cleanup");

	} // testRepetition cicle

	printStatistics(collector, benchmarkResults.string());
	delete collector;
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

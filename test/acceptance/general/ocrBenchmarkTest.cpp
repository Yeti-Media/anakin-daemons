/*
 * ocrBenchmarkTest.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <boost/filesystem/operations.hpp>
#include <processing/commandrunner/OCRDemo.hpp>
#include <pthread.h>
#include <test/acceptance/TestTools.hpp>
#include <test/acceptance/TestDefinitions.hpp>
#include <boost/filesystem/path.hpp>
#include <unistd.h>
#include <utils/Files.hpp>
#include <cstdlib>
#include <iostream>
#include <string>

/**
 * This test are designed to OCR performance test.
 */
void ocrBenchmarkTest(int argc, const char * argv[],
		StatisticsCollector* collector) {

	//--------------------------------------------------------------
	//  Test Setup
	//--------------------------------------------------------------
	string testName = "OCR_Benchmark";

	fs::path ramDir("/tmp/ram/Anakin/test/" + testName);
	if (!fs::is_directory(ramDir)) {
		fs::create_directories(ramDir);
	}

	// INPUTS
	fs::path testDir = argv[1];
	fs::path examplesDir = testDir / "examples";
	fs::path ocrTest = examplesDir / "ocr";

	fs::path dataset = ocrTest / "dataset_images";
	fs::path testLocalization = ocrTest / "text_localization";
	fs::path wordRecognition = ocrTest / "word_recognition";

	// OUTPUTS
	fs::path outputs = ramDir / "outputs";
	fs::path logsDir = ramDir / "logs";
	if (!fs::is_directory(logsDir)) {
		fs::create_directories(logsDir);
	}
	fs::path logsOCR_Demo = logsDir / "OCR_Demo_log.txt";
	fs::path lastStderr = logsDir / "lastStderr.txt";
	fs::path lastStdout = logsDir / "lastStdout.txt";

	printTestMsj(testName, 0);

	//dir cleanups
	dirCleanup(logsDir);
	dirCleanup(outputs);

	//--------------------------------------------------------------
	//  Step 1 - OCR demo startup and test
	//--------------------------------------------------------------
	printStep(testName, 1);

	pthread_t * thread = NULL;
	thread = runDaemonProgram<OCRDemo>(
			"-oLogFile " + pathToAnakinPath(logsOCR_Demo)
					+ " -iHTTP 8080 -oHTTP -verbose");

	//check if the server start
	bool serverStarted = false;
	while (!serverStarted) {
		sleep(2);
		std::string * capture = get_file_contents(logsOCR_Demo.c_str());
		if (capture->find("* Server started *") != std::string::npos) {
			serverStarted = true;
		}
	}

	//repeated "query" times, to obtain solid benchmarks
	for (int query = 1; query <= 10; query++) {
		string cmd =
				"time curl -X POST -H \"Content-Type: application/json\" -d '{\"action\":\"ocr\", \"ocr\":\""
						+ dataset.string()
						+ "/img_3.png\"}' --connect-timeout 10  -lv http://127.0.0.1:8080/ > ";
		command(collector, true, cmd
		+ pathToAnakinPath(lastStdout) + " 2> " + pathToAnakinPath(lastStderr),
				true);
		cout << "* Request number " << query << endl;

	}
	stopAnakinHTTP(thread, logsDir, collector);
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

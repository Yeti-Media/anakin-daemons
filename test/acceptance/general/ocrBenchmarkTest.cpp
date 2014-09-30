/*
 * ocrBenchmarkTest.cpp
 *
 *  Created on: 21/7/2014
 *      Author: Franco Pellegrini
 */

#include "../CompileConfigurations.hpp"

#if COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING

#include <test/utils/TextLocator.hpp>
#include <boost/filesystem/operations.hpp>
#include <processing/commandrunner/OCR.hpp>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <output/JSON.h>
#include <output/JSONValue.h>
#include <test/acceptance/TestTools.hpp>
#include <test/acceptance/TestDefinitions.hpp>
#include <boost/filesystem/path.hpp>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>

namespace fs = boost::filesystem;
using namespace Anakin;

/**
 * This test are designed to OCR performance test.
 */
void ocrBenchmarkTest(int argc, const char * argv[]) {

	//--------------------------------------------------------------
	//  Test Setup
	//--------------------------------------------------------------
	string testName = "OCR_Benchmark";

	fs::path ramDir("/tmp/ram/Anakin/AcceptanceTests/" + testName);
	if (!fs::is_directory(ramDir)) {
		fs::create_directories(ramDir);
	}

	int maxTestRepetition = 1;

	// INPUTS
	fs::path testDir = argv[1];
	fs::path examplesDir = testDir / "examples";
	fs::path ocrTest = examplesDir / "ocr";

	fs::path dataset = ocrTest / "dataset_images";
	fs::path testLocalization = ocrTest / "text_localization";
	fs::path wordRecognition = ocrTest / "word_recognition";

	boost::filesystem::path classifier = testDir / "examples" / "ocr"
			/ "classifier";

	boost::filesystem::path trained_classifierNM1 = classifier
			/ "trained_classifierNM1.xml";
	boost::filesystem::path trained_classifierNM2 = classifier
			/ "trained_classifierNM1.xml";
	boost::filesystem::path OCRHMM_transitions_table = classifier
			/ "OCRHMM_transitions_table.xml";
	boost::filesystem::path OCRHMM_knn_model_data = classifier
			/ "OCRHMM_knn_model_data.xml.gz";
	boost::filesystem::path classifier_erGrouping = classifier
			/ "trained_classifier_erGrouping.xml";

	// OUTPUTS
	fs::path outputs = ramDir / "outputs";
	fs::path logsDir = ramDir / "logs";
	if (!fs::is_directory(logsDir)) {
		fs::create_directories(logsDir);
	}
	if (!fs::is_directory(outputs)) {
		fs::create_directories(outputs);
	}

	fs::path logsOCR_Demo = logsDir / "OCR_Demo_log.txt";
	fs::path benchmarkResults = ramDir / "benchmarkResults.txt";
	fs::path precisionResults = ramDir / "precisionResults.txt";

	printTestMsj(testName, 0);

	//dir cleanups
	dirCleanup(logsDir);
	dirCleanup(outputs);

	ofstream oFileStreamPrecisionResults;

	oFileStreamPrecisionResults.open(precisionResults.string(), ios::out);
	if (!oFileStreamPrecisionResults.is_open()) {
		cerr << "File can't be opened: " << precisionResults.string() << endl;
		exitWithError();
	}

	StatisticsCollector * collector = new StatisticsCollector();

	//--------------------------------------------------------------
	//  Step 1 - OCR demo startup and test
	//--------------------------------------------------------------
	printStep(testName, 1);
	cout << "Parsing text localization files...";
	TextLocator locator(testLocalization);
	cout << " OK" << endl;

	//--------------------------------------------------------------
	//  Step 2 - OCR demo startup and test
	//--------------------------------------------------------------
	printStep(testName, 2);

	pthread_t * thread = NULL;
	thread = runDaemonProgram<OCR>(
			"-oLogFile " + pathToAnakinPath(logsOCR_Demo)
					+ " -iHTTP 8080 -oHTTP -threads 2 -verbose -numocrs 10 "
							"-classifierNM1 \"" + trained_classifierNM1.string()
					+ "\" -classifierNM2 \"" + trained_classifierNM2.string()
					+ "\" -OCRHMMtransitions \""
					+ OCRHMM_transitions_table.string() + "\" -OCRHMMknn \""
					+ OCRHMM_knn_model_data.string()
					+ "\" -classifier_erGrouping \""
					+ classifier_erGrouping.string() + "\"");

	//check if the server start
	bool serverStarted = false;
	while (!serverStarted) {
		sleep(2);
		Ptr<string> capture = get_file_contents(logsOCR_Demo.c_str());
		if (capture->find("* Server started *") != string::npos) {
			serverStarted = true;
		}
	}

	list<fs::path> * filesToTest = get_file_list_from(dataset);

	double totalError = 0;
	//repeated "query" times, to obtain solid benchmarks
	for (int testRepetition = 1; testRepetition <= maxTestRepetition;
			testRepetition++) {
		for (list<fs::path>::iterator file = filesToTest->begin();
				file != filesToTest->end(); ++file) {

			//-------------------------------------------------------------
			// Obtaining OCR results
			//-------------------------------------------------------------

			string results;
			string JSONcmd = "{\"action\":\"ocr\", \"ocr\":\""
					+ (*file).string() + "\"}";
			runCURL_JSON(collector, JSONcmd, "OCR", results);

			//Analyzing output
			string pattern2 = "\"error_type\"";
			if (results.find(pattern2) != string::npos) {
				cerr << "OCR replied with an error:" << endl << endl << results
						<< endl << endl;
				stopAnakinHTTP(thread, logsDir, NULL);
				exitWithError();
			}

			if (testRepetition == 1) {
				//-------------------------------------------------------------
				// Comparing OCR results with real text on the image
				//-------------------------------------------------------------
				string filename = "gt_"
						+ (*file).filename().replace_extension(".txt").string();
				fs::path pathToLocationFile = testLocalization / filename;
				string * realText = locator.getLectureFrom(pathToLocationFile);

				if (realText == NULL) {
					cerr << "can't find the file with text locations:" << endl
							<< endl << pathToLocationFile.string() << endl
							<< endl;
					stopAnakinHTTP(thread, logsDir, NULL);
					exitWithError();
				} else {

					oFileStreamPrecisionResults
							<< "______________________________________________________________________"
							<< endl << "* File: " << (*file).string() << endl
							<< "Comparing original text: " << endl
							<< (*realText) << endl << "**** vs ****" << endl;

					//-------------------------------------------------------------
					// JSON parsing (extracting the OCR text result value only)
					//-------------------------------------------------------------
					uint pos = results.find(":[{\"text\":\"");
					if (pos == results.npos) {
						cerr << endl
								<< "can't parse JSON, missing \" :[{\"text\":\" \" on:"
								<< endl << results << endl << endl;
						stopAnakinHTTP(thread, logsDir, NULL);
						exitWithError();
					}
					pos = results.find(":", pos + 2);
					if (pos == results.npos) {
						cerr << endl
								<< "can't parse JSON, missing \":\" on \" :[{\"text\":\" \" in:"
								<< endl << results << endl << endl;
						stopAnakinHTTP(thread, logsDir, NULL);
						exitWithError();
					}
					pos += 2; //final position of the first letter on the text value

					uint textSize = results.find("\"}]}]}", pos);
					if (textSize == results.npos) {
						cerr << endl
								<< "can't parse JSON, missing \" \"}]}]} \" in:"
								<< endl << results << endl << endl;
						stopAnakinHTTP(thread, logsDir, NULL);
						exitWithError();
					}

					textSize -= pos; //final size of the text to extract

					string OCRtext = results.substr(pos, textSize);
					oFileStreamPrecisionResults << OCRtext << endl;
					int distance = stringutils::levenshteinDistance(*realText,
							OCRtext);

					oFileStreamPrecisionResults << "* Levenshtein Distance = "
							<< distance << endl;
					double error = (distance * 100) / (*realText).size();
					oFileStreamPrecisionResults << "* Error = " << error << "%"
							<< endl;
					totalError += error;
					delete realText;
				}
				oFileStreamPrecisionResults.flush();
			} //if (testRepetition == 1)
			cout << "* Repetition number " << testRepetition << endl;
		}
	}
	stopAnakinHTTP(thread, logsDir, collector);

	//Total error calculations
	totalError = totalError / filesToTest->size();
	oFileStreamPrecisionResults << endl
			<< "======================================================================\n"
			<< "* Total error (avg) = " << totalError << "%" << endl;
	oFileStreamPrecisionResults.flush();

	cout << endl
			<< "======================================================================\n"
			<< "* Total error (avg) = " << totalError << "%" << endl
			<< "* Precision detailed results printed on file: "
			<< precisionResults.string() << endl;

	printStatistics(collector, benchmarkResults.string());

	delete collector;
}

#endif  /*COMPILE_MODE == COMPILE_FOR_BIN_ACCEPTANCE_TESTING*/

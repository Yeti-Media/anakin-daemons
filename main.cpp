//================================================================
// PLEASE configure the project in CompileConfigurations.hpp file before
// compile.
//================================================================

#include <connection/Daemon.hpp>
#include <CompileConfigurations.hpp>
#include <processing/Flags.hpp>
#include <processing/Program.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace Anakin;

//=======================================================================================
// Include setup
//=======================================================================================

// *** PATTERN ****

#if COMPILE_MODULE == PATTERNMATCHER || COMPILE_MODULE == ALLMODULES
#include <processing/commandrunner/PatternMatcher.hpp>
#endif

#if COMPILE_MODULE == PATTERNTRAINER || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/PatternTrainer.hpp>
#endif

#if COMPILE_MODULE == PATTERNEXTRACTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/PatternExtractor.hpp>
#endif

#if COMPILE_MODULE == PATTERNDBCONNECTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/PatternDBConnector.hpp>
#endif

// *** FACE RECOGNITION ****

#if COMPILE_MODULE == FACEMATCHER || COMPILE_MODULE == ALLMODULES
#include <processing/commandrunner/FaceMatcher.hpp>
#endif

#if COMPILE_MODULE == FACETRAINER || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/FaceTrainer.hpp>
#endif

#if COMPILE_MODULE == FACEEXTRACTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/FaceExtractor.hpp>
#endif

#if COMPILE_MODULE == FACEDBCONNECTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/FaceDBConnector.hpp>
#endif

// *** FACE RECOGNITION ****

#if COMPILE_MODULE == OCRDEMO || COMPILE_MODULE == ALLMODULES
#include <processing/commandrunner/OCRDemo.hpp>
#endif

// *** TESTS ***

#if COMPILE_MODULE == MATCHERCACHETEST || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/MatcherCacheTest.hpp>
#endif

#if COMPILE_MODE == COMPILE_FOR_PRODUCTION
//=======================================================================================
// MAIN for COMPILE_FOR_PRODUCTION
//=======================================================================================

int main(int argc, const char * argv[]) {

	vector<string> input(0);
	for (int i = 1; i < argc; i++) {
		input.push_back(argv[i]);
	}

	int signal = EXIT_SUCCESS;

	// *** TESTS ***

#if COMPILE_MODULE == MATCHERCACHETEST
	Program* program = new MatcherCacheTest();
#endif

	// *** PATTERN ****

#if COMPILE_MODULE == PATTERNMATCHER
	Program* program = new Daemon<PatternMatcher>();
#endif
#if COMPILE_MODULE == PATTERNDBCONNECTOR
	Program* program = new PatternDBConnector();
#endif
#if COMPILE_MODULE == PATTERNEXTRACTOR
	Program* program = new PatternExtractor();
#endif
#if COMPILE_MODULE == PATTERNTRAINER
	Program* program = new PatternTrainer();
#endif

	// *** FACE RECOGNITION ****

#if COMPILE_MODULE == FACEMATCHER
	Program* program = new Daemon<FaceMatcher>();
#endif
#if COMPILE_MODULE == FACEDBCONNECTOR
	Program* program = new FaceDBConnector();
#endif
#if COMPILE_MODULE == FACEEXTRACTOR
	Program* program = new FaceExtractor();
#endif
#if COMPILE_MODULE == FACETRAINER
	Program* program = new FaceTrainer();
#endif

	// *** OCR ****

#if COMPILE_MODULE == OCRDEMO
	Program* program = new Daemon<OCRDemo>();
#endif

	// common startup

	signal = program->start(&input);
	delete program;

	exit(signal);
}

#endif

#if COMPILE_MODE == COMPILE_FOR_README_UPDATE
//=======================================================================================
// MAIN for COMPILE_FOR_README_UPDATE
//=======================================================================================
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <fstream>

namespace fs = boost::filesystem;
using namespace std;

template<class SpecificProgram>
void exportHelp(fs::path* path) {
	SpecificProgram program;

	string readmeName = "README-" + program.getProgramName() + ".txt";
	fs::path oFile = *path / readmeName;

	ofstream oFileStream;
	if (oFileStream.is_open()) {
		oFileStream.close();
	}
	oFileStream.open(oFile.string(), ios::out);
	if (!oFileStream.is_open()) {
		cerr << oFile << " file can't be opened" << endl;
		exit(EXIT_FAILURE);
	}

	oFileStream << program.getFullTextHelp();
	oFileStream.close();
}

int main(int argc, const char * argv[]) {

	if (argc != 2) {
		cout << "usage: " << argv[0] << " <output-dir>" << endl;
		exit(EXIT_FAILURE);
	}

	fs::path path = argv[1];

	if (!fs::is_directory(path)) {
		cout << " directory " << path << " not found" << endl;
		exit(EXIT_FAILURE);
	}

	exportHelp<MatcherCacheTest>(&path);

	exportHelp<Daemon<PatternMatcher>>(&path);
	exportHelp<PatternDBConnector>(&path);
	exportHelp<PatternExtractor>(&path);
	exportHelp<PatternTrainer>(&path);

	exportHelp<Daemon<FaceMatcher>>(&path);
	exportHelp<FaceDBConnector>(&path);
	exportHelp<FaceExtractor>(&path);
	exportHelp<FaceTrainer>(&path);

	exportHelp<OCRDemo>(&path);

	cout << "Success" << endl;

	exit(EXIT_SUCCESS);
}

#endif

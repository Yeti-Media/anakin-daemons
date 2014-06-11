//================================================================
// PLEASE configure the project in CompileConfigurations.hpp file before
// compile.
//================================================================

#include <connection/Daemon.hpp>
#include <CompileConfigurations.hpp>
#include <processing/Flags.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace Anakin;

//=======================================================================================
// Include setup
//=======================================================================================

#if COMPILE_MODULE == PATTERNMATCHING || COMPILE_MODULE == ALLMODULES
#include <processing/commandrunner/PatternMatchingCommandRunner.hpp>
#endif

#if COMPILE_MODULE == TRAINER || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/SimpleProgramTrainer.hpp>
#endif

#if COMPILE_MODULE == EXTRACTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/SimpleProgramExtractor.hpp>
#endif

#if COMPILE_MODULE == DBCONNECTOR || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/SimpleProgramDBConnector.hpp>
#endif

#if COMPILE_MODULE == MATCHERCACHE || COMPILE_MODULE == ALLMODULES
#include <processing/simpleprogram/SimpleProgramMatcherCache.hpp>
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

#if COMPILE_MODULE == PATTERNMATCHING
	Daemon<PatternMatchingCommandRunner>* daemon = new Daemon<
	PatternMatchingCommandRunner>();
	signal = daemon->start(&input, true);
	delete daemon;
#endif
#if COMPILE_MODULE == MATCHERCACHE
	SimpleProgram* matcherCache = new SimpleProgramMatcherCache();
	signal = matcherCache->run(&input);
	delete matcherCache;
#endif
#if COMPILE_MODULE == DBCONNECTOR
	SimpleProgram* dbConnector = new SimpleProgramDBConnector();
	signal = dbConnector->run(&input);
	delete dbConnector;
#endif
#if COMPILE_MODULE == EXTRACTOR
	SimpleProgram* extractor = new SimpleProgramExtractor();
	signal = extractor->run(&input);
	delete extractor;
#endif
#if COMPILE_MODULE == TRAINER
	SimpleProgram* trainer = new SimpleProgramTrainer();
	signal = trainer->run(&input);
	delete trainer;
#endif
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

template<class Program>
void exportHelp(fs::path* path) {
	Program program;

	string readmeName = "README-" + program.getProgramName() + ".txt";
	fs::path oFile = *path / readmeName;

	ofstream oFileStream;
	if (oFileStream.is_open()) {
		oFileStream.close();
	}
	oFileStream.open(oFile.string(), ios::out | ios::app);
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

	exportHelp<Daemon<PatternMatchingCommandRunner>>(&path);
	exportHelp<SimpleProgramMatcherCache>(&path);
	exportHelp<SimpleProgramDBConnector>(&path);
	exportHelp<SimpleProgramExtractor>(&path);
	exportHelp<SimpleProgramTrainer>(&path);

	cout << "Success" << endl;

	exit(EXIT_SUCCESS);
}

#endif

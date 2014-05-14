//================================================================
// PLEASE configure the project in CompileConfigurations.hpp file before
// compile.
//================================================================
#include <CompileConfigurations.hpp>

#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>

using namespace Anakin;

void logProgramArguments(int argc, const char * argv[]) {
	std::string startComand;
	for (int i = 1; i < argc; i++) {
		startComand = startComand + " " + argv[i];
	}
	LOG_F("Args")<< startComand;
}

void Anakin::initModuleFlags(Flags* flags) {
	flags->setNoValuesFlag("modepatternmatching");
	flags->setNoValuesFlag("modematchercache");
	flags->setNoValuesFlag("modedbconnector");
	flags->setNoValuesFlag("modeextractor");
	flags->setNoValuesFlag("modetrainer");

	flags->setIncompatibility("modepatternmatching", "modematchercache");
	flags->setIncompatibility("modepatternmatching", "modedbconnector");
	flags->setIncompatibility("modepatternmatching", "modeextractor");
	flags->setIncompatibility("modepatternmatching", "modetrainer");

	flags->setIncompatibility("modematchercache", "modedbconnector");
	flags->setIncompatibility("modematchercache", "modeextractor");
	flags->setIncompatibility("modematchercache", "modetrainer");

	flags->setIncompatibility("modedbconnector", "modeextractor");
	flags->setIncompatibility("modedbconnector", "modetrainer");

	flags->setIncompatibility("modeextractor", "modetrainer");
}

//=======================================================================================
#if COMPILE_MODULE == PATTERNMATCHING || COMPILE_MODULE == ALLMODULES
//=======================================================================================
#include "processing/AnakinFlags.hpp"
#include "output/DataOutput.hpp"
#include "connection/Socket.hpp"
#include "connection/ATCPSocket.hpp"
#include "connection/AUDPSocket.hpp"
#include "connection/Server.hpp"
#include "connection/RequestServer.hpp"
#include "connection/DelimiterBasedTCPSocket.hpp"
#include "connection/DTCPServerSocket.hpp"
#include "connection/HTTPSocket.hpp"

#define CONSOLE 1
#define TCP     2
#define UDP     4
#define DTCP    8
#define HTTP    16

int Anakin::patternMatching(int argc, const char * argv[]) {
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);
	std::cerr.tie(nullptr);
	char iMode = CONSOLE;
	char oMode = CONSOLE;
	unsigned short portIn = 18003;
	std::string ipOut = "127.0.0.1";
	std::string portOut = "18002";
	std::string logFile = "anakin.log";
	bool verbose = false;
	CacheConfig cacheConfig;
	CacheConfig * pCacheConfig = &cacheConfig;

	vector<string> *input = new vector<string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	Flags* anakinInput = new Flags();
	anakinInput->setOverridingFlag("help");
	anakinInput->setNoValuesFlag("verbose");

	//GENERAL CACHE CONFIG
	anakinInput->setOptionalFlag("cacheLoadingTimeWeight");
	anakinInput->setNoValuesFlag("cacheDiscardLessValuable");
	anakinInput->setOptionalFlag("cacheSize");
	anakinInput->setOptionalFlag("cacheLife");
	anakinInput->setOptionalFlag("cacheScenesSize");
	anakinInput->setOptionalFlag("cacheScenesLife");

	//INPUT
	anakinInput->setNoValuesFlag("iConsole");
	anakinInput->setOptionalFlag("iTCP");
	anakinInput->setOptionalFlag("iUDP");
	anakinInput->setOptionalFlag("iDTCP");
	anakinInput->setOptionalFlag("iHTTP");
	anakinInput->setIncompatibility("iConsole", "iTCP");
	anakinInput->setIncompatibility("iConsole", "iUDP");
	anakinInput->setIncompatibility("iConsole", "iDTCP");
	anakinInput->setIncompatibility("iConsole", "iHTTP");
	anakinInput->setIncompatibility("iUDP", "iTCP");
	anakinInput->setIncompatibility("iUDP", "iDTCP");
	anakinInput->setIncompatibility("iUDP", "iHTTP");
	anakinInput->setIncompatibility("iTCP", "iDTCP");
	anakinInput->setIncompatibility("iTCP", "iHTTP");
	anakinInput->setIncompatibility("iDTCP", "iHTTP");

	initModuleFlags(anakinInput);

	//OUTPUT
	anakinInput->setNoValuesFlag("oConsole");
	anakinInput->setOptionalFlag("oLogFile");
	anakinInput->setOptionalFlag("oTCP");
	anakinInput->setOptionalFlag("oUDP");
	anakinInput->setOptionalFlag("oDTCP");
	anakinInput->setNoValuesFlag("oHTTP");
	anakinInput->setIncompatibility("oConsole", "oTCP");
	anakinInput->setIncompatibility("oConsole", "oUDP");
	anakinInput->setIncompatibility("oConsole", "oDTCP");
	anakinInput->setIncompatibility("oUDP", "oTCP");
	anakinInput->setIncompatibility("oUDP", "oDTCP");
	anakinInput->setIncompatibility("oTCP", "oDTCP");
	anakinInput->setIncompatibility("oHTTP", "oConsole");
	anakinInput->setIncompatibility("oHTTP", "oTCP");
	anakinInput->setIncompatibility("oHTTP", "oDTCP");
	anakinInput->setIncompatibility("oHTTP", "oDTCP");

	anakinInput->setDependence("iHTTP", "oHTTP");
	anakinInput->setDependence("oHTTP", "iHTTP");

	anakinInput->setMinCount(2);
	anakinInput->setVerbose(true);

	if (anakinInput->validateInput(input)) {
		if (anakinInput->flagFound("help")) {

			anakinInput->setOptionalFlag("cacheLoadingTimeWeight");
			anakinInput->setOptionalFlag("cacheNoDiscardLessValuable");
			anakinInput->setOptionalFlag("cacheSize");
			anakinInput->setOptionalFlag("cacheLife");
			anakinInput->setOptionalFlag("cacheScenesSize");
			anakinInput->setOptionalFlag("cacheScenesLife");

			cout << "Anakin help\n\n" << "usage : ./anakin2 -help\n"
					<< "usage : ./anakin2 [cacheLoadingTimeWeight|cacheDiscardLessValuable|cacheSize|cacheLife|cacheScenesSize|cacheScenesLife] (-iConsole|(-iTCP|iUDP <port>)) (-oConsole|-oLogFile|(-oTCP|oUDP <ip> <port>))\n"
					<< "-iConsole/oConsole         : use console to input or output respectively\n"
					<< "-oLogFile                  : path to the output logging file\n"
					<< "-iTCP/oTCP <ip> <port>     : use a TCP connection with ip and port for input or output respectively\n"
					<< "-iUDP/oUDP <ip> <port>     : use a UDP connection with ip and port for input or output respectively\n"
					<< "-cacheLoadingTimeWeight    : (default 9) how many importance the loading time of a trainer will influence on his life\n"
					<< "-cacheNoDiscardLessValuable: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache\n"
					<< "-cacheSize                 : (default 10) trainers cache size (how many trainers can be stored in the cache)\n"
					<< "-cacheLife                 : (default 1) trainers starting life\n"
					<< "-cacheScenesSize           : (default 15) scenes cache size (how many scenes can be stored in the cache)\n"
					<< "-cacheScenesLife           : (default 10) scenes starting life";
			exit(EXIT_SUCCESS);
		}
		if (anakinInput->flagFound("verbose")) {
			verbose = true;
		}

		vector<string>* values;

		//CACHE CONFIG
		if (anakinInput->flagFound("cacheLoadingTimeWeight")) {
			values = anakinInput->getFlagValues("cacheLoadingTimeWeight");
			if (values->size() == 1) {
				cacheConfig.cacheLoadingTimeWeight = stoi(values->at(0));
			} else {
				cout << "param cacheLoadingTimeWeight needs only one value"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("cacheNoDiscardLessValuable")) {
			cacheConfig.cacheNoDiscardLessValuable = true;
		}

		if (anakinInput->flagFound("cacheSize")) {
			values = anakinInput->getFlagValues("cacheSize");
			if (values->size() == 1) {
				cacheConfig.cacheSize = stoi(values->at(0));
			} else {
				cout << "param cacheSize needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("cacheLife")) {
			values = anakinInput->getFlagValues("cacheLife");
			if (values->size() == 1) {
				cacheConfig.cacheLife = stoi(values->at(0));
			} else {
				cout << "param cacheLife needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("cacheScenesSize")) {
			values = anakinInput->getFlagValues("cacheScenesSize");
			if (values->size() == 1) {
				cacheConfig.cacheScenesSize = stoi(values->at(0));
			} else {
				cout << "param cacheScenesSize needs only one value"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("cacheScenesLife")) {
			values = anakinInput->getFlagValues("cacheScenesLife");
			if (values->size() == 1) {
				cacheConfig.cacheScenesLife = stoi(values->at(0));
			} else {
				cout << "param cacheScenesLife needs only one value"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}

		//INPUT
		if (anakinInput->flagFound("iConsole")) {
			iMode = CONSOLE;
		}
		if (anakinInput->flagFound("iTCP")) {
			iMode = TCP;
			values = anakinInput->getFlagValues("iTCP");
			if (values->size() == 1) {
				portIn = stoi(values->at(0));
			} else {
				cout << "param iTCP needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("iUDP")) {
			iMode = UDP;
			values = anakinInput->getFlagValues("iUDP");
			if (values->size() == 1) {
				portIn = stoi(values->at(0));
			} else {
				cout << "param iUDP needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("iDTCP")) {
			iMode = DTCP;
			values = anakinInput->getFlagValues("iDTCP");
			if (values->size() == 1) {
				portIn = stoi(values->at(0));
			} else {
				cout << "param iDTCP needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("iHTTP")) {
			iMode = HTTP;
			values = anakinInput->getFlagValues("iHTTP");
			if (values->size() == 1) {
				portIn = stoi(values->at(0));
			} else {
				cout << "param iHTTP needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		//OUTPUT
		if (anakinInput->flagFound("oConsole")) {
			oMode = CONSOLE;
		}
		if (anakinInput->flagFound("oLogFile")) {
			values = anakinInput->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (anakinInput->flagFound("oTCP")) {
			oMode = TCP;
			values = anakinInput->getFlagValues("oTCP");
			if (values->size() == 2) {
				ipOut = values->at(0);
				portOut = values->at(1);
			} else {
				cout << "param oTCP needs two values" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("oUDP")) {
			oMode = UDP;
			values = anakinInput->getFlagValues("oUDP");
			if (values->size() == 2) {
				ipOut = values->at(0);
				portOut = values->at(1);
			} else {
				cout << "param oUDP needs two values" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("oDTCP")) {
			oMode = DTCP;
			values = anakinInput->getFlagValues("oDTCP");
			if (values->size() == 2) {
				ipOut = values->at(0);
				portOut = values->at(1);
			} else {
				cout << "param oDTCP needs two values" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (anakinInput->flagFound("oHTTP")) {
			oMode = HTTP;
		}
	} else {
		cout << "Input error!" << std::endl;
		exit(EXIT_FAILURE);
	}

	//logger initialization
	Logging::OutputPolicyFile::SetFileStream(logFile);
	logProgramArguments(argc, argv);

	Socket* soutput;
	HTTPSocket* httpSocket;
	if (oMode & TCP) {
		soutput = new ATCPSocket(ipOut, portOut);
	} else if (oMode & UDP) {
		soutput = new AUDPSocket(ipOut, portOut);
	} else if (oMode & DTCP) {
		soutput = new DelimiterBasedTCPSocket(ipOut, portOut, "<line>",
				"<stop>");
	}
	if ((oMode & TCP) || (oMode & UDP) || (oMode & DTCP)) {
		soutput->connect();
	}

	AnakinFlags* aflags = new AnakinFlags();

	Server* server = new RequestServer(pCacheConfig, portIn, 10, 4, verbose,
			iMode, "<line>", "<end>");

	DataOutput* output;
	if (oMode & CONSOLE) {
		output = new DataOutput();
	} else if (oMode & HTTP) {
		httpSocket = server->getHttpSocket();
		output = new DataOutput(httpSocket);
	} else {
		output = new DataOutput(soutput);
	}

	server->start(aflags, output);

	exit(EXIT_SUCCESS);
}

//=======================================================================================
#endif
#if COMPILE_MODULE == MATCHERCACHE || COMPILE_MODULE == ALLMODULES
//=======================================================================================

#include <iostream>
#include "db/DBDriver.hpp"
#include "processing/SFBMCache.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"
#include <string>
#include <vector>
#include <ctime>

void Anakin::showHelpMatcherCache() {

}

/**
 * For testing purposes
 */
int Anakin::matcherCache(int argc, const char * argv[]) {
	DBDriver* dbdriver = new DBDriver();
	if (!dbdriver->connect()) {
		std::cerr << dbdriver->getMessage() << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << dbdriver->getMessage() << std::endl;
	CacheConfig cacheConfig;
	CacheConfig * pCacheConfig = &cacheConfig;
	cacheConfig.cacheLoadingTimeWeight = 9;
	SFBMCache* cache = new SFBMCache(dbdriver, pCacheConfig);
	SerializableFlannBasedMatcher* sfbm;
	std::vector<int> patternsID;
	patternsID.push_back(5);
	patternsID.push_back(6);
	srand(time(0));
	uint requests = 45;
	for (uint r = 0; r < requests; r++) {
		int id = rand() % 2;
		bool matcherError = false;
		sfbm = cache->loadMatcher(patternsID.at(id), &matcherError);
		std::cout << "loaded matcher(" << patternsID.at(id) << "), matcher is "
				<< (sfbm->empty() ? "empty" : "not empty") << std::endl;
		std::cout << "current hit ratio: " << cache->getHitRatio()
				<< " | current miss ratio: " << cache->getMissRatio()
				<< std::endl;
	}
	cache->printLoadCount();
	exit(EXIT_SUCCESS);
}

//=======================================================================================
#endif
#if COMPILE_MODULE == DBCONNECTOR || COMPILE_MODULE == ALLMODULES
//=======================================================================================

#include "db/DBDriver.hpp"
#include "db/DBUser.hpp"
#include "db/DBPattern.hpp"
#include "db/DBHistogram.hpp"
#include "utils/XMLoader.hpp"
#include "utils/Constants.hpp"
#include "data/ImageInfo.hpp"
#include <iostream>
#include "utils/Help.hpp"

void Anakin::showHelpDbConnector() {
	Help* h = new Help();
	std::cout << h->getFullHelp() << std::endl;
}

int Anakin::dbConnector(int argc, const char * argv[]) {
	std::string path;
	std::string logFile = "anakin-dbtest.log";
	bool saveObjects = false;
	int userID;
	bool saveUser = false;
	bool showhelp = false;
	char objectsAs = 0;
	bool load = false;
	std::string smatcher_id;
	bool savePatterns = false;
	int sceneID = -1;
	bool loadingScenes = false;

	vector<string> *input = new vector<string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	Flags* flags = new Flags();
	flags->setMinCount(1);
	flags->setOverridingFlag("help");
	flags->setOptionalFlag("oLogFile");
	flags->setOptionalFlag("user");
	flags->setOptionalFlag("path");
	flags->setNoValuesFlag("patterns");
	flags->setNoValuesFlag("scenes");
	flags->setNoValuesFlag("histograms");
	flags->setNoValuesFlag("landscapes");
	flags->setOptionalFlag("index");
	flags->setNoValuesFlag("savePatterns");

	initModuleFlags(flags);

	vector<string>* pathLooseDeps = new vector<string>(0);
	pathLooseDeps->push_back("patterns");
	pathLooseDeps->push_back("histograms");
	pathLooseDeps->push_back("landscapes");
	pathLooseDeps->push_back("savePatterns");
	pathLooseDeps->push_back("scenes");
	flags->setLooseDependencies("path", pathLooseDeps);
	flags->setIncompatibility("index", "path");
	flags->setIncompatibility("patterns", "histograms");
	flags->setIncompatibility("patterns", "landscapes");
	flags->setIncompatibility("patterns", "scenes");
	flags->setIncompatibility("histograms", "landscapes");
	flags->setIncompatibility("histograms", "scenes");
	flags->setIncompatibility("landscapes", "scenes");
	flags->setIncompatibility("index", "patterns");
	flags->setIncompatibility("index", "histograms");
	flags->setIncompatibility("index", "landscapes");
	flags->setIncompatibility("index", "scenes");
	flags->setNoValuesFlag("load");
	vector<string>* loadLooseDeps = new vector<string>(0);
	loadLooseDeps->push_back("user");
	loadLooseDeps->push_back("index");
	loadLooseDeps->push_back("patterns");
	loadLooseDeps->push_back("histograms");
	loadLooseDeps->push_back("landscapes");
	loadLooseDeps->push_back("scenes");
	flags->setLooseDependencies("load", loadLooseDeps);
	flags->setIncompatibility("load", "path");
	flags->setDependence("savePatterns", "index");
	flags->setIncompatibility("savePatterns", "load");
	flags->setOptionalFlag("sceneID");
	flags->setDependence("sceneID", "load");
	flags->setDependence("sceneID", "scenes");

	if (flags->validateInput(input)) {
		if (flags->flagFound("help")) {
			showhelp = true;
		}
		std::vector<std::string>* values;
		if (flags->flagFound("oLogFile")) {
			values = flags->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("user")) {
			saveUser = true;
			values = flags->getFlagValues("user");
			if (values->size() == 1) {
				userID = std::stoi(values->at(0));
			} else {
				std::cerr << "param user need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("path")) {
			saveObjects = true;
			values = flags->getFlagValues("path");
			if (values->size() == 1) {
				path = values->at(0);
			} else {
				std::cerr << "param path need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("patterns")) {
			objectsAs = Constants::PATTERN;
		}
		if (flags->flagFound("histograms")) {
			objectsAs = Constants::HISTOGRAM;
		}
		if (flags->flagFound("landscapes")) {
			objectsAs = Constants::LANDSCAPE;
		}
		if (flags->flagFound("index")) {
			objectsAs = Constants::INDEX;
		}
		if (flags->flagFound("scenes")) {
			objectsAs = Constants::SCENE;
		}
		if (flags->flagFound("load")) {
			load = true;
			if (objectsAs == Constants::SCENE) {
				loadingScenes = true;
			}
		}
		if (flags->flagFound("sceneID")) {
			values = flags->getFlagValues("sceneID");
			if (values->size() == 1) {
				sceneID = std::stoi(values->at(0));
			} else {
				std::cerr << "param sceneID need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("savePatterns")) {
			savePatterns = true;
		}

		if (flags->flagFound("index")) {
			saveObjects = true;
			values = flags->getFlagValues("index");
			if (values->size() == 1 && load) {
				smatcher_id = values->at(0);
			} else if (values->size() == 2 && !load) {
				smatcher_id = values->at(0);
				userID = std::stoi(values->at(1));
			} else {
				std::cerr
						<< "param index need two values when saving and one value when loading"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}

		if (loadingScenes && sceneID == -1)   //THIS MUST BE AT THE END
				{
			std::cerr << "Missing sceneID flag!" << std::endl;
			exit(EXIT_FAILURE);
		}
	} else {
		std::cerr << "Input error!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (showhelp) {
		showHelpDbConnector();
		exit(EXIT_SUCCESS);
	}

	//logger initialization
	Logging::OutputPolicyFile::SetFileStream(logFile);
	logProgramArguments(argc, argv);

	DBDriver* driver = new DBDriver();
	driver->connect();
	std::cout << driver->getMessage() << std::endl;
	LOG_F("Info")<< driver->getMessage();

	DBUser* user;
	std::vector<DBPattern*>* patterns;
	std::vector<DBHistogram*>* histograms;
	std::vector<DBHistogram*>* landscapes;
	if (load) {
		if (objectsAs != Constants::INDEX && objectsAs != Constants::SCENE)
			user = new DBUser(userID);
		bool error = false;
		switch (objectsAs) {
		case Constants::PATTERN: {
			std::vector<int> pattern_ids = driver->getUserPatterns(
					user->getID(), &error);
			if (error) {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR")<< driver->getMessage();
				exit(EXIT_FAILURE);
			}
			for (uint i = 0; i < pattern_ids.size(); i++) {
				bool patternError = false;
				DBPattern* pattern;
				if (!driver->retrievePattern(pattern_ids.at(i), &patternError,
				true, &pattern)) {
					std::cerr << driver->getMessage() << std::endl;
					LOG_F("ERROR")<< driver->getMessage();
					exit(EXIT_FAILURE);
				}
				//std::cout << "loaded pattern with label : " << pattern->getLabel() << " and id : " << pattern->getID() << std::endl;
			}
			std::cout << "user " << user->getID() << " have "
					<< pattern_ids.size() << " patterns" << std::endl;
			LOG_F("Info")<< "user " << user->getID() << " have "
			<< pattern_ids.size() << " patterns";
			break;
		}
		case Constants::HISTOGRAM: {
			std::vector<int> histogram_ids = driver->getUserHistograms(
					user->getID(), &error);
			if (error) {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR")<< driver->getMessage();
				exit(EXIT_FAILURE);
			}
			for (uint i = 0; i < histogram_ids.size(); i++) {
				bool histogramError = false;
				DBHistogram* histogram;
				if (!driver->retrieveHistogram(histogram_ids.at(i),
						&histogramError, true, &histogram)) {
					std::cerr << driver->getMessage() << std::endl;
					LOG_F("ERROR")<< driver->getMessage();
					exit(EXIT_FAILURE);
				}
				std::cout << "loaded histogram : " << histogram->getID()
						<< std::endl;
				LOG_F("Info")<< "loaded histogram : " << histogram->getID();
			}
			break;
		}
		case Constants::LANDSCAPE: {
			std::vector<int> landscape_ids = driver->getUserLandscapes(
					user->getID(), &error);
			if (error) {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR")<< driver->getMessage();
				exit(EXIT_FAILURE);
			}
			for (uint i = 0; i < landscape_ids.size(); i++) {
				bool histogramError = false;
				DBHistogram* landscape;
				if (!driver->retrieveHistogram(landscape_ids.at(i),
						&histogramError, true, &landscape)) {
					std::cerr << driver->getMessage() << std::endl;
					LOG_F("ERROR")<< driver->getMessage();
					exit(EXIT_FAILURE);
				}
				std::cout << "loaded landscape : " << landscape->getID()
						<< std::endl;
				LOG_F("Info")<<"loaded landscape : " << landscape->getID();
			}
			break;
		}
		case Constants::INDEX: {
			int trainerID = std::stoi(smatcher_id);
			bool SFBMError = false;
			if (driver->retrieveSFBM(trainerID, &SFBMError)) {
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR") << driver->getMessage();
				exit(EXIT_FAILURE);
			}
			break;
		}
		case Constants::SCENE: {
			ImageInfo* scene;
			bool sceneError = false;
			if (driver->retrieveScene(&scene, sceneID, &sceneError)) {
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR") << driver->getMessage();
				exit(EXIT_FAILURE);
			}
			break;
		}
		}
		exit(EXIT_SUCCESS);
	}
	if (saveUser) {
		user = new DBUser(userID);
	}
	if (saveObjects) {
		XMLoader* loader;
		if (objectsAs != Constants::INDEX)
			loader = new XMLoader(path);
		switch (objectsAs) {
		case Constants::PATTERN: {
			patterns = loader->loadAsPattern();
			if (saveUser) {
				for (uint p = 0; p < patterns->size(); p++) {
					user->addPattern(patterns->at(p));
				}
				driver->saveUserPatterns(user, true);
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < patterns->size(); p++) {
					driver->savePattern(patterns->at(p));
					std::cout << driver->getMessage() << std::endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			break;
		}
		case Constants::HISTOGRAM: {
			histograms = loader->loadAsHistogram();
			if (saveUser) {
				for (uint p = 0; p < histograms->size(); p++) {
					user->addHistogram(histograms->at(p));
				}
				driver->saveUserHistograms(user, true);
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < histograms->size(); p++) {
					driver->saveHORL(histograms->at(p), true);
					std::cout << driver->getMessage() << std::endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			break;
		}
		case Constants::LANDSCAPE: {
			landscapes = loader->loadAsLandscape();
			if (saveUser) {
				for (uint p = 0; p < landscapes->size(); p++) {
					user->addLandscape(landscapes->at(p));
				}
				driver->saveUserLandscapes(user, true);
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < landscapes->size(); p++) {
					driver->saveHORL(landscapes->at(p), true);
					std::cout << driver->getMessage() << std::endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			break;
		}
		case Constants::INDEX: {
			int trainer_id;
			if (driver->storeSFBM(smatcher_id, &trainer_id, userID, true,
			false)) {
				std::cout << driver->getMessage() << std::endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				std::cerr << driver->getMessage() << std::endl;
				LOG_F("ERROR") << driver->getMessage();
				exit(EXIT_FAILURE);
			}
			if (savePatterns) {
				bool error;
				std::vector<int> patterns = driver->getUserPatterns(userID,
						&error);
				if (error) {
					std::cerr << driver->getMessage() << std::endl;
					LOG_F("ERROR")<< driver->getMessage();
					exit(EXIT_FAILURE);
				}
				for (uint p = 0; p < patterns.size(); p++) {
					if (driver->storeNthPattern(trainer_id, p,
							patterns.at(p))) {
						std::cout << driver->getMessage() << std::endl;
						LOG_F("Info")<< driver->getMessage();
					} else {
						std::cerr << driver->getMessage() << std::endl;
						LOG_F("ERROR") << driver->getMessage();
					}
				}
			}
			break;
		}
		case Constants::SCENE: {
			patterns = loader->loadAsPattern();
			for (uint s = 0; s < patterns->size(); s++) {
				DBPattern* sceneAsDBPattern = patterns->at(s);
				if (driver->storeScene(sceneAsDBPattern)) {
					std::cout << driver->getMessage() << std::endl;
					LOG_F("Info")<< driver->getMessage();
				} else {
					std::cerr << driver->getMessage() << std::endl;
					LOG_F("ERROR") << driver->getMessage();
					exit(EXIT_FAILURE);
				}
			}
			break;
		}
		}
	} else {
		driver->saveUser(user);
		std::cout << driver->getMessage() << std::endl;
		LOG_F("Info")<< driver->getMessage();
	}

	exit(EXIT_SUCCESS);
}

//=======================================================================================
#endif
#if COMPILE_MODULE == EXTRACTOR || COMPILE_MODULE == ALLMODULES
//=======================================================================================

#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>               // for std::cout
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "data/ImageDataInput.hpp"
#include "data/PatternLoader.hpp"
#include "data/SingleImageDataInput.hpp"
#include "data/HistogramComparator.hpp"
#include "data/Histogram.hpp"
#include "data/HistogramsIO.hpp"

#define PATTERNS 1
#define HISTOGRAMS 2
#define LANDSCAPE 4

#define COLOR 1
#define GRAY 2
#define HSV 4
#define MINMAX 8    //FOR LATER USE
#define AVG 16      //FOR LATER USE

namespace fs = boost::filesystem;

void Anakin::showHelpExtractor() {
	cout << "Extractor help\n" << std::endl;
	cout << "usage: ./extractor -help";
	cout
			<< "usage: ./extractor [oLogFile] (I/O arguments) (paths arguments) ((landscape arguments)|(histograms arguments))\n"
			<< std::endl;
	cout << "where I/O arguments are:" << std::endl;
	cout << "-oLogFile  : path to the output logging file" << std::endl
			<< "-landscape : will construct histograms to use with landscape detection"
			<< std::endl;
	cout << "-histograms : will construct one histogram per pattern"
			<< std::endl;
	cout << "-patterns : will extract descriptors and keypoints" << std::endl;
	cout << "" << std::endl;
	cout << "where paths arguments are:" << std::endl;
	cout
			<< "-iFile|iFolder <path> : sets the input as a file or folder path respectively"
			<< std::endl;
	cout
			<< "-oPath <path> : sets the path for where to store the serialized data"
			<< std::endl;
	cout << "-toJson : this will make output serialized data to the console"
			<< std::endl;
	cout << "" << std::endl;
	cout << "where landscape exclusive arguments are:" << std::endl;
	cout
			<< "-label <label> : the label to use when serializing landscape histogram"
			<< std::endl;
	cout << "where landscape and histogram's common arguments are:"
			<< std::endl;
	cout << "-color : will use RGB to generate histograms and landscape"
			<< std::endl;
	cout << "-gray : will use grayscale to generate histograms and landscape"
			<< std::endl;
	cout
			<< "-hsv : will use hue and saturation to generate histograms and landscape"
			<< std::endl;
}

int Anakin::extractor(int argc, const char * argv[]) {
	std::string logFile = "anakin-extractor.log";
	bool useInputPathAsDir = false;
	char mode = 0;
	char inputMode = 0;
	std::string label = "";
	std::string inputDir = "";
	string outputDir = "";
	bool showhelp = false;
	bool saveToFile = true;
	bool useYaml = true;
	bool loadOnDemand = false;

	vector<string> *input = new vector<string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	Flags* flags = new Flags();
	flags->setMinCount(1);
	flags->setOverridingFlag("help");
	flags->setOptionalFlag("oLogFile");

	initModuleFlags(flags);
	//I/O MODE
	flags->setNoValuesFlag("landscape");
	flags->setNoValuesFlag("histograms");

	flags->setNoValuesFlag("matching");
	flags->setIncompatibility("landscape", "histograms");
	flags->setIncompatibility("landscape", "matching");
	flags->setIncompatibility("histograms", "matching");
	//I/O paths
	flags->setOptionalFlag("iFile");
	flags->setOptionalFlag("iFolder");
	flags->setIncompatibility("iFile", "iFolder");
	flags->setOptionalFlag("oPath");
	vector<string>* oPathLooseDeps = new vector<string>(0);
	oPathLooseDeps->push_back("iFile");
	oPathLooseDeps->push_back("iFolder");
	flags->setLooseDependencies("oPath", oPathLooseDeps);
	flags->setIncompatibility("landscape", "iFile");
	flags->setNoValuesFlag("toJson");
	flags->setIncompatibility("toJson", "oPath");
	vector<string>* imodesLooseDeps = new vector<string>(0);
	imodesLooseDeps->push_back("oPath");
	imodesLooseDeps->push_back("toJson");
	flags->setLooseDependencies("landscape", imodesLooseDeps);
	flags->setLooseDependencies("histograms", imodesLooseDeps);
	flags->setLooseDependencies("matching", imodesLooseDeps);

	//HISTOGRAMS/LANDSCAPE MODE
	flags->setNoValuesFlag("color");
	flags->setNoValuesFlag("hsv");
	flags->setNoValuesFlag("gray");
	vector<string>* modesLooseDeps = new vector<string>(0);
	modesLooseDeps->push_back("color");
	modesLooseDeps->push_back("hsv");
	modesLooseDeps->push_back("gray");
	flags->setLooseDependencies("landscape", modesLooseDeps);
	flags->setLooseDependencies("histograms", modesLooseDeps);
	flags->setIncompatibility("color", "matching");
	flags->setIncompatibility("gray", "matching");
	flags->setIncompatibility("hsv", "matching");
	//LANDSCAPE EXTRA OPTIONS
	flags->setOptionalFlag("label");
	flags->setDependence("label", "landscape");
	flags->setDependence("landscape", "label");
	//FORMAT OPTIONS
	flags->setNoValuesFlag("xml");
	flags->setNoValuesFlag("yml");
	flags->setIncompatibility("xml", "yml");
	vector<string>* formatLooseDeps = new vector<string>(0);
	formatLooseDeps->push_back("matching");
	formatLooseDeps->push_back("landscape");
	formatLooseDeps->push_back("histograms");
	//LOAD ON DEMAND
	flags->setNoValuesFlag("lod");

	flags->setVerbose(true);

	if (flags->validateInput(input)) {
		vector<string>* values;
		if (flags->flagFound("help")) {
			showhelp = true;
		}
		if (flags->flagFound("oLogFile")) {
			values = flags->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("landscape")) {
			inputMode = LANDSCAPE;
		}
		if (flags->flagFound("histograms")) {
			inputMode = HISTOGRAMS;
		}
		if (flags->flagFound("matching")) {
			inputMode = PATTERNS;
		}
		if (flags->flagFound("iFile")) {
			useInputPathAsDir = false;
			values = flags->getFlagValues("iFile");
			if (values->size() == 1) {
				inputDir = values->at(0);
			} else {
				cerr << "param iFile need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("iFolder")) {
			useInputPathAsDir = true;
			values = flags->getFlagValues("iFolder");
			if (values->size() == 1) {
				inputDir = values->at(0);
			} else {
				cerr << "param iFolder need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("oPath")) {
			values = flags->getFlagValues("oPath");
			if (values->size() == 1) {
				outputDir = values->at(0);
			} else {
				cerr << "param oPath need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("label")) {
			values = flags->getFlagValues("label");
			if (values->size() == 1) {
				label = values->at(0);
			} else {
				cerr << "param label need only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("color")) {
			mode = mode | COLOR;
		}
		if (flags->flagFound("gray")) {
			mode = mode | GRAY;
		}
		if (flags->flagFound("hsv")) {
			mode = mode | HSV;
		}
		if (flags->flagFound("toJson")) {
			saveToFile = false;
		}
		if (flags->flagFound("xml")) {
			useYaml = false;
		}
		if (flags->flagFound("yml")) {
			useYaml = true;
		}
		if (flags->flagFound("lod")) {
			loadOnDemand = true;
		}
	} else {
		cerr << "input error!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if (showhelp) {
		showHelpExtractor();
		exit(EXIT_SUCCESS);
	}

	//logger initialization
	Logging::OutputPolicyFile::SetFileStream(logFile);
	logProgramArguments(argc, argv);

	cv::Ptr<cv::FeatureDetector> fdetector;
	cv::Ptr<cv::DescriptorExtractor> dextractor;

	std::vector<RichImg*> patterns;
	DataInput* patternsDataInput;
	PatternLoader* patternsLoader;

	if (!useInputPathAsDir) {
		patternsDataInput = new SingleImageDataInput(inputDir);
	} else {
		patternsDataInput = new ImageDataInput(inputDir, loadOnDemand);
	}

	if (inputMode & PATTERNS) {
		fdetector = new cv::SurfFeatureDetector(400);
		dextractor = new cv::SurfDescriptorExtractor();
		patternsLoader = new PatternLoader(patternsDataInput, patterns,
				fdetector, dextractor);
		char mode = useYaml ? PatternLoader::YAML : PatternLoader::XML;
		patternsLoader->load_and_save(outputDir, saveToFile, mode);
		exit(EXIT_SUCCESS);
	} else {
		patternsLoader = new PatternLoader(patternsDataInput, patterns,
				fdetector, dextractor);
		patternsLoader->load();
	}

	HistogramsIO* io = new HistogramsIO(outputDir);
	HistogramComparator* hComparator = new HistogramComparator(patterns, io);
	if (useYaml) {
		mode = mode | HistogramComparator::YAML;
	} else {
		mode = mode | HistogramComparator::XML;
	}
	if (inputMode & HISTOGRAMS) {
		hComparator->makeAndSaveHistograms(mode, saveToFile);
	} else if (inputMode & LANDSCAPE) {
		hComparator->makeAndSaveLandscape(mode, label, saveToFile);
	}

	exit(EXIT_SUCCESS);
}

//=======================================================================================
#endif
#if COMPILE_MODULE == TRAINER || COMPILE_MODULE == ALLMODULES
//=======================================================================================

#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>               // for std::cout
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "processing/Trainer.hpp"
#include "data/PatternLoader.hpp"
#include "processing/BasicFlannTrainer.hpp"
#include "processing/SerializedPatternDataInput.hpp"
#include "data/PatternLoader.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"

using namespace cv;

void Anakin::showHelpTrainer() {
	std::cout << "trainer help" << std::endl;
	std::cout << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "./trainer -help" << std::endl;
	std::cout
			<< "./trainer [oLogFile] -user <userID> -saveToFile [<folder>] <filename>"
			<< std::endl;
	std::cout
			<< "./trainer [oLogFile] -patternsId <id id ...> -saveToFile [<folder>] <filename>"
			<< std::endl;
	std::cout << "flag -oLogFile  : path to the output logging file"
			<< std::endl;
	std::cout << std::endl;
}

int Anakin::trainer(int argc, const char * argv[]) {
	std::string logFile = "anakin-trainer.log";
	std::string userID;
	//char mode = 0;
	std::string folder;
	std::string fileName;
	std::vector<int>* patternsId;
	bool user = false;

	//FLAGS DECLARATION
	Flags* flags = new Flags();
	flags->setMinCount(2);
	flags->setOverridingFlag("help");
	flags->setOptionalFlag("oLogFile");
	initModuleFlags(flags);
	flags->setOptionalFlag("user");
	//flags->setNoValuesFlag("patterns");
	flags->setOptionalFlag("patternsId");
	flags->setOptionalFlag("saveToFile");

	//INPUT VALIDATION
	std::vector<std::string> *input = new std::vector<std::string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	if (flags->validateInput(input)) {
		if (flags->flagFound("help")) {
			showHelpTrainer();
			exit(EXIT_SUCCESS);
		}
		std::vector<std::string>* values;
		if (flags->flagFound("oLogFile")) {
			values = flags->getFlagValues("oLogFile");
			if (values->size() == 1) {
				logFile = values->at(0);
			} else {
				cout << "param oLogPath needs one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("user")) {
			values = flags->getFlagValues("user");
			user = true;
			if (values->size() == 1) {
				userID = values->at(0);
			} else {
				std::cerr << "flag user needs only one value" << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("patternsId")) {
			values = flags->getFlagValues("patternsId");
			if (!values->empty()) {
				patternsId = new vector<int>(0);
				for (unsigned int i = 0; i < values->size(); i++) {
					(*patternsId).push_back(std::stoi(values->at(i)));
				}
			} else {
				std::cerr << "flag patternsId needs at least one value"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}
		if (flags->flagFound("saveToFile")) {
			values = flags->getFlagValues("saveToFile");
			if (values->size() == 1) {
				folder = "";
				fileName = values->at(0);
			} else if (values->size() == 2) {
				folder = values->at(0);
				fileName = values->at(1);
			} else {
				std::cerr << "flag saveToFile needs between one and two values"
						<< std::endl;
				exit(EXIT_FAILURE);
			}
		}
	} else {
		std::cerr << "Input error!" << std::endl;
		exit(EXIT_FAILURE);
	}

	//logger initialization
	Logging::OutputPolicyFile::SetFileStream(logFile);
	logProgramArguments(argc, argv);

	//TRAINING
	const Ptr<flann::IndexParams>& indexParams = new flann::KDTreeIndexParams(
			4);
	const Ptr<flann::SearchParams>& searchParams = new flann::SearchParams();
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::Ptr<FlannBasedMatcher>(
			new SerializableFlannBasedMatcher(indexParams, searchParams));
	matcher->clear();
	std::vector<RichImg*> patterns;
	SerializedPatternDataInput* sinput;
	if (user) {
		sinput = new SerializedPatternDataInput(userID);
	} else {
		sinput = new SerializedPatternDataInput(patternsId);
	}
	PatternLoader* loader = new PatternLoader(sinput, patterns);
	loader->load();

	Trainer* trainer = new BasicFlannTrainer(matcher, patterns, folder,
			fileName);
	trainer->train_and_save();

	exit(EXIT_SUCCESS);
}

#endif

#if COMPILE_MODE == COMPILE_FOR_PRODUCTION
//=======================================================================================
// MAIN for COMPILE_FOR_PRODUCTION
//=======================================================================================

int main(int argc, const char * argv[]) {
#if COMPILE_MODULE == PATTERNMATCHING
	return patternMatching(argc, argv);
#endif
#if COMPILE_MODULE == MATCHERCACHE
	return matcherCache(argc,argv);
#endif
#if COMPILE_MODULE == DBCONNECTOR
	return dbConnector(argc,argv);
#endif
#if COMPILE_MODULE == EXTRACTOR
	return extractor(argc,argv);
#endif
#if COMPILE_MODULE == TRAINER
	return trainer(argc, argv);
#endif
}

#endif

#if COMPILE_MODE == COMPILE_FOR_TESTING
//=======================================================================================
// MAIN for COMPILE_FOR_TESTING
//=======================================================================================

int main(int argc, const char * argv[]) {

	vector<string> *input = new vector<string>(0);
	for (int i = 1; i < argc; i++) {
		input->push_back(argv[i]);
	}

	Flags* anakinInput = new Flags();
	anakinInput->setVerbose(true);
	anakinInput->setIgnoreUnknownFlags(true);
	initModuleFlags(anakinInput);

	if (anakinInput->validateInput(input)) {
		if (anakinInput->flagFound("modepatternmatching")) {
			return patternMatching(argc, argv);
		} else if (anakinInput->flagFound("modematchercache")) {
			return matcherCache(argc, argv);
		} else if (anakinInput->flagFound("modedbconnector")) {
			return dbConnector(argc, argv);
		} else if (anakinInput->flagFound("modeextractor")) {
			return extractor(argc, argv);
		} else if (anakinInput->flagFound("modetrainer")) {
			return trainer(argc, argv);
		} else {
			cout << "Input error! Expected flag "
			<< "-modepatternmatching|-modematchercache|-modedbconnector|-modeextractor|-modetrainer"
			<< std::endl;
			exit(EXIT_FAILURE);
		}
	} else {
		cout << "Input error! Expected flag "
		<< "-modepatternmatching|-modematchercache|-modedbconnector|-modeextractor|-modetrainer"
		<< std::endl;
		exit(EXIT_FAILURE);
	}

}

#endif

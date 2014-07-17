/*
 * Daemon.h
 *
 *  Created on: 16/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef DAEMON_H_
#define DAEMON_H_

#include <connection/RequestServer.hpp>
#include <connection/Server.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutput.hpp>
#include <processing/Flags.hpp>
#include <processing/Program.hpp>
#include <processing/SFBMCache.hpp>
#include <sys/types.h>
#include <utils/help/Help.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

namespace Anakin {

/**
 * A daemon is a program that receive commands and process them. It use a server
 * to listen the incoming messages. A daemon use implementations of a CommandRunner
 * to process a message.
 */
template<class SpecificCommandRunner>
class Daemon: public Program {
public:
	Daemon();
	virtual ~Daemon();

	string getProgramName();
	Help* getHelp();

protected:
	int run(vector<string> *input);
	void initProgramFlags();
};

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::Daemon() {
	// TODO Auto-generated constructor stub

}

template<class SpecificCommandRunner>
void Daemon<SpecificCommandRunner>::initProgramFlags() {
	//SERVER CONFIGS
	programFlags.setOptionalFlag("pghost");
	programFlags.setOptionalFlag("pgport");
	programFlags.setOptionalFlag("dbName");
	programFlags.setOptionalFlag("login");
	programFlags.setOptionalFlag("pwd");

	programFlags.setDependence("pghost", "pgport");
	programFlags.setDependence("pghost", "dbName");
	programFlags.setDependence("pghost", "login");
	programFlags.setDependence("pghost", "pwd");

	programFlags.setDependence("pgport", "pghost");
	programFlags.setDependence("pgport", "dbName");
	programFlags.setDependence("pgport", "login");
	programFlags.setDependence("pgport", "pwd");

	programFlags.setDependence("dbName", "pgport");
	programFlags.setDependence("dbName", "pghost");
	programFlags.setDependence("dbName", "login");
	programFlags.setDependence("dbName", "pwd");

	programFlags.setDependence("login", "pgport");
	programFlags.setDependence("login", "pghost");
	programFlags.setDependence("login", "dbName");
	programFlags.setDependence("login", "pwd");

	programFlags.setDependence("pwd", "pgport");
	programFlags.setDependence("pwd", "pghost");
	programFlags.setDependence("pwd", "dbName");
	programFlags.setDependence("pwd", "login");

	//GENERAL CACHE CONFIG
	programFlags.setOptionalFlag("cacheLoadingTimeWeight");
	programFlags.setNoValuesFlag("cacheDiscardLessValuable");
	programFlags.setOptionalFlag("cacheSize");
	programFlags.setOptionalFlag("cacheLife");
	programFlags.setOptionalFlag("cacheScenesSize");
	programFlags.setOptionalFlag("cacheScenesLife");

	//GENERAL CONFIGS
	programFlags.setOptionalFlag("threads");
	programFlags.setOptionalFlag("queueCapacity");

	//INPUT
	programFlags.setNoValuesFlag("iConsole");
	programFlags.setOptionalFlag("iHTTP");
	programFlags.setIncompatibility("iConsole", "iHTTP");

	//OUTPUT
	programFlags.setNoValuesFlag("oConsole");
	programFlags.setOptionalFlag("oLogFile");
	programFlags.setNoValuesFlag("oHTTP");
	programFlags.setIncompatibility("oHTTP", "oConsole");
	programFlags.setDependence("iHTTP", "oHTTP");
	programFlags.setDependence("oHTTP", "iHTTP");

	programFlags.setMinCount(2);
	programFlags.setVerbose(true);
}

template<class SpecificCommandRunner>
string Daemon<SpecificCommandRunner>::getProgramName() {
	SpecificCommandRunner commandRunner;
	return commandRunner.getProgramName();
}

template<class SpecificCommandRunner>
Help* Daemon<SpecificCommandRunner>::getHelp() {
	SpecificCommandRunner commandRunner;
	return commandRunner.getHelp();
}

template<class SpecificCommandRunner>
int Daemon<SpecificCommandRunner>::run(vector<string> *input) {
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	cerr.tie(nullptr);
	char iMode = Server<SpecificCommandRunner>::CONSOLE;
	char oMode = Server<SpecificCommandRunner>::CONSOLE;

	unsigned int httpPort = 18003;

	string pghost = "";
	string pgport = "";
	string dbName = "";
	string login = "";
	string pwd = "";

	unsigned int threads = 4;
	unsigned int queueCapacity = 10;

	CacheConfig cacheConfig;
	CacheConfig * pCacheConfig = &cacheConfig;

	//______________________________________________________________________//
	//                         FLAGS PARSING                                //
	//______________________________________________________________________//

	vector<string>* values = NULL;

	//POSTGRES config

	if (programFlags.flagFound("pghost")) {
		values = programFlags.getFlagValues("pghost");
		if (values->size() == 1) {
			pghost = values->at(0);
		} else {
			cout << "param pghost needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("pgport")) {
		values = programFlags.getFlagValues("pgport");
		if (values->size() == 1) {
			pgport = values->at(0);
		} else {
			cout << "param pgport needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("dbName")) {
		values = programFlags.getFlagValues("dbName");
		if (values->size() == 1) {
			dbName = values->at(0);
		} else {
			cout << "param dbName needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("login")) {
		values = programFlags.getFlagValues("login");
		if (values->size() == 1) {
			login = values->at(0);
		} else {
			cout << "param login needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("pwd")) {
		values = programFlags.getFlagValues("pwd");
		if (values->size() == 1) {
			pwd = values->at(0);
		} else {
			cout << "param pwd needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	//CACHE CONFIG
	if (programFlags.flagFound("cacheLoadingTimeWeight")) {
		values = programFlags.getFlagValues("cacheLoadingTimeWeight");
		if (values->size() == 1) {
			cacheConfig.cacheLoadingTimeWeight = stoi(values->at(0));
		} else {
			cout << "param cacheLoadingTimeWeight needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheNoDiscardLessValuable")) {
		cacheConfig.cacheNoDiscardLessValuable = true;
	}

	if (programFlags.flagFound("cacheSize")) {
		values = programFlags.getFlagValues("cacheSize");
		if (values->size() == 1) {
			cacheConfig.cacheSize = stoi(values->at(0));
		} else {
			cout << "param cacheSize needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheLife")) {
		values = programFlags.getFlagValues("cacheLife");
		if (values->size() == 1) {
			cacheConfig.cacheLife = stoi(values->at(0));
		} else {
			cout << "param cacheLife needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheScenesSize")) {
		values = programFlags.getFlagValues("cacheScenesSize");
		if (values->size() == 1) {
			cacheConfig.cacheScenesSize = stoi(values->at(0));
		} else {
			cout << "param cacheScenesSize needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheScenesLife")) {
		values = programFlags.getFlagValues("cacheScenesLife");
		if (values->size() == 1) {
			cacheConfig.cacheScenesLife = stoi(values->at(0));
		} else {
			cout << "param cacheScenesLife needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	//THREADS
	if (programFlags.flagFound("threads")) {
		values = programFlags.getFlagValues("threads");
		if (values->size() == 1) {
			threads = stoi(values->at(0));
		} else {
			cout << "param threads needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	//BLOCKING CAPACITY QUEUE
	if (programFlags.flagFound("queueCapacity")) {
		values = programFlags.getFlagValues("queueCapacity");
		if (values->size() == 1) {
			queueCapacity = stoi(values->at(0));
		} else {
			cout << "param queueCapacity needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	//INPUT
	if (programFlags.flagFound("iConsole")) {
		iMode = Server<SpecificCommandRunner>::CONSOLE;
	}

	if (programFlags.flagFound("iHTTP")) {
		iMode = Server<SpecificCommandRunner>::HTTP;
		values = programFlags.getFlagValues("iHTTP");
		if (values->size() == 1) {
			httpPort = stoi(values->at(0));
		} else {
			cout << "param iHTTP needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	//OUTPUT
	if (programFlags.flagFound("oConsole")) {
		oMode = Server<SpecificCommandRunner>::CONSOLE;
	}
	if (programFlags.flagFound("oHTTP")) {
		oMode = Server<SpecificCommandRunner>::HTTP;
	}

	//______________________________________________________________________//
	//                         DAEMON INITIALIZATION                        //
	//______________________________________________________________________//

	HTTPSocket* httpSocket;

	Server<SpecificCommandRunner>* server = new RequestServer<
			SpecificCommandRunner>(pCacheConfig, iMode, pghost, pgport, dbName,
			login, pwd, httpPort, queueCapacity, threads, verbose);

	DataOutput* output;
	if (oMode & Server<SpecificCommandRunner>::CONSOLE) {
		output = new DataOutput();
	} else if (oMode & Server<SpecificCommandRunner>::HTTP) {
		httpSocket = server->getHttpSocket();
		output = new DataOutput(httpSocket);
	} else {
		cerr << "unknown output mode \"" << oMode << "\"" << endl;
		return EXIT_FAILURE;
	}

	server->start(output);

	delete output;
	delete server;
	return EXIT_SUCCESS;
}

template<class SpecificCommandRunner>
Daemon<SpecificCommandRunner>::~Daemon() {
}

}
;
/* namespace Anakin */

#endif /* DAEMON_H_ */

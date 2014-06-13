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

	string getFullTextHelp();
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
string Daemon<SpecificCommandRunner>::getFullTextHelp() {
	SpecificCommandRunner commandRunner;
	Help* help = commandRunner.getHelp();
	string output =
			commandRunner.getProgramName() + "\n\nDaemon arguments:\n"
					+ "[cacheLoadingTimeWeight <int>|cacheDiscardLessValuable <bool>|cacheSize <int>|cacheLife <int>|cacheScenesSize <int>|cacheScenesLife <int>|-oLogFile <path>|-threads <int>|-queueCapacity <int>|(pghost <name> pgport <port> dbName <name> login <user> pwd <password>)] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)"
					+ "\n\nFlags:\n\n"
					+ "-iConsole/oConsole			: use console to input or output respectively\n"
					+ "-oLogFile <path>			: path to the output logging file. If it's ignored, logging will be skipped.\n"
					+ "-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input\n"
					+ "-pghost <name> 				: name of host to connect to. If this begins with a slash, it specifies Unix-domain communication rather than TCP/IP communication; the value is the name of the directory in which the socket file is stored. The default behavior when host is not specified is to connect to a Unix-domain socket in /tmp (or whatever socket directory was specified when PostgreSQL was built). On machines without Unix-domain sockets, the default is to connect to localhost. Empty string will use system environment defaults.\n"
					+ "-pgport <port>				: port number to connect to at the server host, or socket file name extension for Unix-domain connections. Empty string will use system environment defaults.\n"
					+ "-dbName <name>				: the database name. Defaults to be the same as the user name. Empty string will use system environment defaults.\n"
					+ "-login <user>				: postgreSQL user name to connect as. Defaults to be the same as the operating system name of the user running the application. Empty string will use system environment defaults.\n"
					+ "-pwd <password>				: password to be used if the server demands password authentication. Empty string will use system environment defaults.\n"
					+ "-cacheLoadingTimeWeight <int>		: (default 9) how many importance the loading time of a trainer will influence on his life\n"
					+ "-cacheNoDiscardLessValuable <bool>	: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache\n"
					+ "-cacheSize <int>			: (default 10) trainers cache size (how many trainers can be stored in the cache)\n"
					+ "-cacheLife <int>			: (default 1) trainers starting life\n"
					+ "-cacheScenesSize <int>			: (default 15) scenes cache size (how many scenes can be stored in the cache)\n"
					+ "-cacheScenesLife <int>			: (default 10) scenes starting life\n"
					+ "-threads <int>				: (default 4) threads to use at processing requests\n"
					+ "-queueCapacity <int>			: (default 10) processing queue max capacity\n"
					+ "\n" + help->getFullHelp();
	delete help;
	return output;
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

	vector<string>* values = new vector<string>();

	//POSTGRES config

	if (programFlags.flagFound("pghost")) {
		values->clear();
		values = programFlags.getFlagValues("pghost");
		if (values->size() == 1) {
			pghost = values->at(0);
		} else {
			cout << "param pghost needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("pgport")) {
		values->clear();
		values = programFlags.getFlagValues("pgport");
		if (values->size() == 1) {
			pgport = values->at(0);
		} else {
			cout << "param pgport needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("dbName")) {
		values->clear();
		values = programFlags.getFlagValues("dbName");
		if (values->size() == 1) {
			dbName = values->at(0);
		} else {
			cout << "param dbName needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("login")) {
		values->clear();
		values = programFlags.getFlagValues("login");
		if (values->size() == 1) {
			login = values->at(0);
		} else {
			cout << "param login needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("pwd")) {
		values->clear();
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
		values->clear();
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
		values->clear();
		values = programFlags.getFlagValues("cacheSize");
		if (values->size() == 1) {
			cacheConfig.cacheSize = stoi(values->at(0));
		} else {
			cout << "param cacheSize needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheLife")) {
		values->clear();
		values = programFlags.getFlagValues("cacheLife");
		if (values->size() == 1) {
			cacheConfig.cacheLife = stoi(values->at(0));
		} else {
			cout << "param cacheLife needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheScenesSize")) {
		values->clear();
		values = programFlags.getFlagValues("cacheScenesSize");
		if (values->size() == 1) {
			cacheConfig.cacheScenesSize = stoi(values->at(0));
		} else {
			cout << "param cacheScenesSize needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}

	if (programFlags.flagFound("cacheScenesLife")) {
		values->clear();
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
		values->clear();
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
		values->clear();
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

	delete values;

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

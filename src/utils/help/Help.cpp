/*
 * Help.cpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#include <utils/help/Help.hpp>

using namespace std;

namespace Anakin {

Help::Help() {
	daemonIntro =
			"Daemon arguments:\n"
					"[cacheLoadingTimeWeight <int>|cacheDiscardLessValuable <bool>|cacheSize <int>|cacheLife <int>|cacheScenesSize <int>|cacheScenesLife <int>|-oLogFile <path>|-threads <int>|-queueCapacity <int>|(pghost <name> pgport <port> dbName <name> login <user> pwd <password>)] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)"
					"\n\nFlags:\n\n"
					"-iConsole/oConsole			: use console to input or output respectively\n"
					"-oLogFile <path>			: path to the output logging file. If it's ignored, logging will be skipped.\n"
					"-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input\n"
					"-pghost <name> 				: name of host to connect to. If this begins with a slash, it specifies Unix-domain communication rather than TCP/IP communication; the value is the name of the directory in which the socket file is stored. The default behavior when host is not specified is to connect to a Unix-domain socket in /tmp (or whatever socket directory was specified when PostgreSQL was built). On machines without Unix-domain sockets, the default is to connect to localhost. Empty string will use system environment defaults.\n"
					"-pgport <port>				: port number to connect to at the server host, or socket file name extension for Unix-domain connections. Empty string will use system environment defaults.\n"
					"-dbName <name>				: the database name. Defaults to be the same as the user name. Empty string will use system environment defaults.\n"
					"-login <user>				: postgreSQL user name to connect as. Defaults to be the same as the operating system name of the user running the application. Empty string will use system environment defaults.\n"
					"-pwd <password>				: password to be used if the server demands password authentication. Empty string will use system environment defaults.\n"
					"-cacheLoadingTimeWeight <int>		: (default 9) how many importance the loading time of a trainer will influence on his life\n"
					"-cacheNoDiscardLessValuable <bool>	: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache\n"
					"-cacheSize <int>			: (default 10) trainers cache size (how many trainers can be stored in the cache)\n"
					"-cacheLife <int>			: (default 1) trainers starting life\n"
					"-cacheScenesSize <int>			: (default 15) scenes cache size (how many scenes can be stored in the cache)\n"
					"-cacheScenesLife <int>			: (default 10) scenes starting life\n"
					"-threads <int>				: (default 4) threads to use at processing requests\n"
					"-queueCapacity <int>			: (default 10) processing queue max capacity\n"
					"\n";
}

Help::~Help() {
}

/**
 * inheritance placeholder
 */
bool Help::showDaemonHelp() {
	return false;
}

string Help::getFullHelp() {
	if (showDaemonHelp()) {
		return daemonIntro + extendedDaemonFlags + getIntro() + getUsage()
				+ getFlags() + getExamples();
	} else {
		return getIntro() + getUsage() + getFlags() + getExamples();
	}
}

string Help::getIntro() {
	if (!this->intro.empty()) {
		return "Summary:\n\n" + this->intro + "\n";
	} else {
		return "";
	}
}

string Help::getUsage() {
	if (!this->usage.empty()) {
		return "Usage:\n\n" + this->usage + "\n";
	} else {
		return "";
	}
}

string Help::getFlags() {
	if (!this->flags.empty()) {
		return "Flags:\n\n" + this->flags + "\n";
	} else {
		return "";
	}
}

string Help::getExamples() {
	if (!this->examples.empty()) {
		return "Examples:\n\n" + this->examples + "\n";
	} else {
		return "";
	}
}

} /* namespace Anakin */

#include <processing/CommandRunner.hpp>
#include <sys/types.h>
#include <map>
#include <vector>

using namespace Anakin;
using namespace cv;

CommandRunner::CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache) {
	this->flags = flags;
	this->rw = new ResultWriter();
	this->cache = cache;
	this->out = out;
	this->sceneID = -1;
}

void CommandRunner::validateRequest(std::vector<std::string> *input) {

}

/**
 * placeholder for inheritance
 */
void CommandRunner::run(){
}

/**
 * placeholder for inheritance
 */
CommandRunner::~CommandRunner(){
}

string CommandRunner::getHelp() {
	return string(
			"Daemon arguments: [cacheLoadingTimeWeight|cacheDiscardLessValuable|cacheSize|cacheLife|cacheScenesSize|cacheScenesLife|(-oLogFile <path>)] (-iConsole|(-iHTTP <port>)) (-oConsole|-oHTTP)\n\n"
					"-iConsole/oConsole			: use console to input or output respectively\n"
					"-oLogFile					: path to the output logging file\n"
					"-iHTTP <port>/oHTTP			: use a HTTP connection with a port for input\n"
					"-cacheLoadingTimeWeight		: (default 9) how many importance the loading time of a trainer will influence on his life\n"
					"-cacheNoDiscardLessValuable	: (default not defined = true) if a trainer is loaded from the db whose life is less than the object to be dropped then the new object will not be stored in the cache\n"
					"-cacheSize					: (default 10) trainers cache size (how many trainers can be stored in the cache)\n"
					"-cacheLife					: (default 1) trainers starting life\n"
					"-cacheScenesSize			: (default 15) scenes cache size (how many scenes can be stored in the cache)\n"
					"-cacheScenesLife			: (default 10) scenes starting life\n");

}

//PRIVATE

bool CommandRunner::checkDuplicatedIndexes(std::vector<std::string> indexes,
		std::string * duplicated) {
	std::map<std::string, int> visited;
	for (uint i = 0; i < indexes.size(); i++) {
		if (visited.find(indexes.at(i)) != visited.end()) {
			*duplicated = indexes.at(i);
			return false;
		}
		visited[indexes.at(i)] = 0;
	}
	return true;
}

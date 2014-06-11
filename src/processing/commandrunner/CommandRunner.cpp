#include <processing/commandrunner/CommandRunner.hpp>
#include <sys/types.h>
#include <map>
#include <vector>

using namespace Anakin;
using namespace cv;

CommandRunner::CommandRunner() {
	this->flags = NULL;
	this->out = NULL;
	this->sceneID = -1;
}

/**
 * Should initializate all used variables
 */
void CommandRunner::initializeCommandRunner(DataOutput* out) {
	this->flags = new Flags();
	this->out = out;
}

/**
 * Placeholder for inheritance.
 */
Help* CommandRunner::getHelp() {
	return NULL;
}

/**
 * Placeholder for inheritance.
 */
string CommandRunner::getProgramName() {
	return "";
}

/**
 * Placeholder for inheritance.
 */
void CommandRunner::validateRequest(std::vector<std::string> *input) {

}

/**
 * placeholder for inheritance
 */
void CommandRunner::run() {
}

/**
 * placeholder for inheritance
 */
CommandRunner::~CommandRunner() {
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

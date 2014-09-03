#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <processing/commandrunner/CommandRunner.hpp>
#include <sys/types.h>
#include <map>
#include <vector>

using namespace Anakin;
using namespace cv;

namespace fs = boost::filesystem;

CommandRunner::CommandRunner(string programName) {
	this->flags = NULL;
	this->out = NULL;
	this->sceneID = -1;
	this->cache = NULL;
	setProgramName(programName);
}

void CommandRunner::setProgramName(const string & name) {
	assert(!name.empty());
	fs::path temp("/tmp/ram/Anakin/" + name + "_tmpDir");
	if (!fs::is_directory(temp)) {
		fs::create_directories(temp);
	}
	tempDir = temp.string() + "/";
	this->programName = name;
}

/**
 * Should initializate all used variables
 */
void CommandRunner::initializeCommandRunner(DataOutput* out, SFBMCache* cache) {
	this->flags = new Flags();
	this->out = out;
	this->cache = cache;
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
	return programName;
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

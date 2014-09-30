#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <processing/commandrunner/CommandRunner.hpp>
#include <sys/types.h>
#include <cassert>
#include <map>

using namespace Anakin;
using namespace cv;

namespace fs = boost::filesystem;

CommandRunner::CommandRunner(const string & programName,
		const string & threadName) {
	this->sceneID = -1;
	setProgramName(programName);
	this->threadName = threadName;
	this->flags = makePtr<Flags>();
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

Ptr<Flags> CommandRunner::getProgramFlags() {
	return flags;
}

/**
 * Should initializate all used variables
 */
void CommandRunner::initializeCommandRunner(const Ptr<DataOutput> & out,
		const Ptr<SFBMCache> & cache) {
	this->out = out;
	this->cache = cache;
}

/**
 * Placeholder for inheritance.
 */
void CommandRunner::parseServerFlags(const Ptr<Flags> & serverFlags) {
}

/**
 * Placeholder for inheritance.
 */
void CommandRunner::extendServerCommandsWith(const Ptr<Flags> & serverFlags) {
}

/**
 * Placeholder for inheritance.
 */
Ptr<Help> CommandRunner::getHelp() {
	return Ptr<Help>();
}

string CommandRunner::getProgramName() {
	return programName;
}

string CommandRunner::getThreadName() {
	return threadName;
}

/**
 * Placeholder for inheritance.
 */
void CommandRunner::validateRequest(const Ptr<vector<string>> & input) {

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

bool CommandRunner::checkDuplicatedIndexes(const vector<string> & indexes,
		string & duplicated) {
	map<string, int> visited;
	for (uint i = 0; i < indexes.size(); i++) {
		if (visited.find(indexes.at(i)) != visited.end()) {
			duplicated = indexes.at(i);
			return false;
		}
		visited[indexes.at(i)] = 0;
	}
	return true;
}

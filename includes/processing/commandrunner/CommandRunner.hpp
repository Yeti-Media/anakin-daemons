// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
#ifndef COMMANDRUNNER_HPP
#define COMMANDRUNNER_HPP

#include <vector>
#include <opencv2/core.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>               // for cout
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"

#include "matching/BasicFlannDetector.hpp"
#include "output/DataOutput.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "processing/SFBMCache.hpp"
#include "data/ImageInfo.hpp"
#include "matching/FlannMatchingProcessor.hpp"
#include <processing/Flags.hpp>
#include <utils/help/Help.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

/**
 * This abtract class will process a request
 *
 * the process can be defined by the following steps
 *
 * 1 - validate the request using a Flags object
 * 2 - extract flags and values from the request
 * 3 - validate the extracted flags and values
 * 4 - execute an action based on the extracted flags and values
 * 5 - send a response for the request using a DataOutput object
 *
 * See PatternMatchingCommandRunner for an example.
 */
class CommandRunner {
public:
	/**
	 * Constructor (does steps 1, 2, 3)
	 */
	CommandRunner(const string & programName, const string & threadName);

	/**
	 * This command should be implemented if you need to extend the server commands with flags
	 * and to initialize some variables on your CommandRunner implementation.
	 */
	virtual void extendServerCommandsWith(const Ptr<Flags> & serverFlags);

	/**
	 * Parse the extended server flags, and pre-initializate the CommandRunner
	 */
	virtual void parseServerFlags(const Ptr<Flags> & serverFlags);

	/**
	 * Setup the flags, the output and the cache (if used).
	 * out   : used to send responses
	 */
	virtual void initializeCommandRunner(const Ptr<DataOutput> & out,
			const Ptr<SFBMCache> & cache);

	/**
	 * This do the step 1
	 */
	virtual void validateRequest(const Ptr<vector<string>> & input) = 0;

	/**
	 * run the command runner (does steps 4 and 5)
	 */
	virtual void run() = 0;

	/**
	 * Return a full help instance. Must be deleted after use.
	 */
	virtual Ptr<Help> getHelp() = 0;

	/**
	 * Return the name of the program.
	 */
	string getProgramName();

	/**
	 * Return the name of the program.
	 */
	Ptr<Flags> getProgramFlags();

	/**
	 * Return the name of the program.
	 */
	string getThreadName();

	void setProgramName(const string & name);

	virtual ~CommandRunner();

	/**
	 *
	 * @return true if an error occurred in the latest request.
	 *
	 */
	bool isError() { return inputError; }

	/**
	 *
	 * @return a flag that can be used to identify what service to provide,
	 * when pooling multiple services. Returns a blank string if there is
	 * no known flag for this purpose.
	 *
	 */
	string getFlagHint() { return ""; }

protected:
	bool checkDuplicatedIndexes(const vector<string> & indexes,
			string & duplicated);
	int mma = 8;
	float mr = 1.f / 1.5f;
	int sceneID;
	vector<string> indexes;
	string reqID;
	string lastError;
	bool inputError = false;
	Ptr<DataOutput> out;
	Ptr<FlannMatchingProcessor> processor;
	Ptr<BasicFlannDetector> detector;
	Ptr<Flags> flags;
	Ptr<SFBMCache> cache;
	string tempDir;
private:
	string programName;
	string threadName;
};

} /* namespace Anakin */
;

#endif // COMMANDRUNNER_HPP

#include <db/DBDriver.hpp>
#include <db/DBPattern.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/core/operations.hpp>
#include <processing/SerializedPatternDataInput.hpp>
#include <sys/types.h>
#include <utils/ClearVector.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <boost/filesystem.hpp>
#include <utils/files/Files.hpp>

namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;
using namespace cv;

SerializedPatternDataInput::SerializedPatternDataInput(const string & userID,
		const char *pghost, const char *pgport, const char *dbName,
		const char *login, const char *pwd, const string & tmpDir,
		TempDirCleaner * tempDirCleaner) {
	if (!initAndConnectDriver(pghost, pgport, dbName, login, pwd,
			tempDirCleaner)) {
		reportDBDriverError();
		exit(EXIT_FAILURE);
	}
	this->tmpDir = tmpDir;
	this->userID = userID;
	this->cache = makePtr<vector<Ptr<ImageInfo>>>();
	this->loaded = false;
	this->current = -1;
	this->patternsToFind = Ptr<vector<int>>();
}

SerializedPatternDataInput::SerializedPatternDataInput(
		const Ptr<vector<int>> & patternsToFind, const char *pghost, const char *pgport,
		const char *dbName, const char *login, const char *pwd,
		const string & tmpDir, TempDirCleaner * tempDirCleaner) {
	if (!initAndConnectDriver(pghost, pgport, dbName, login, pwd,
			tempDirCleaner)) {
		reportDBDriverError();
		exit(EXIT_FAILURE);
	}
	this->tmpDir = tmpDir;
	this->patternsToFind = patternsToFind;
	this->cache = makePtr<vector<Ptr<ImageInfo>>>();
	this->loaded = false;
	this->current = -1;
}

SerializedPatternDataInput::~SerializedPatternDataInput() {
	//tempDirCleaner shod not be deleted
}

bool SerializedPatternDataInput::nextInput(QuickLZ* quickLZstate,
		Ptr<ImageInfo> & output) {
	if (this->current < 0) {
		if (!this->loaded) {
			this->cache->clear();
			if (!loadDataFromDB(this->cache, quickLZstate)) {
				reportDBDriverError();
				exit(EXIT_FAILURE);
			}
			this->loaded = true;
		}
		this->current = 0;
	}
	if (this->current == this->cache->size()) {
		return false;
	} else {
		output = this->cache->at(this->current);
		this->current++;
		return true;
	}
}

void SerializedPatternDataInput::reload() {
	this->current = -1;
}

//PRIVATE

void SerializedPatternDataInput::loadData(Ptr<vector<Ptr<ImageInfo>>>& data,
const Ptr<string> & rawData) {
	//string xmlData = "<?xml version=\"1.0\"?>";
	//xmlData.append(*rawData);
	Ptr<ImageInfo> ii = makePtr<ImageInfo>();
	//write_to_file(rawData,"/tmp/ram/Anakin/lastXML.xml");
	cv::FileStorage fstorage((*rawData).c_str(),
	cv::FileStorage::READ | cv::FileStorage::MEMORY);
	cv::FileNode n = fstorage.root();
	read(n, ii);
	fstorage.release();
	data->push_back(ii);
}

void SerializedPatternDataInput::read(const cv::FileNode& node, Ptr<ImageInfo> & x) {
	if (node.empty())
		x = makePtr<ImageInfo>();
	else
		x->read(node);
}

bool SerializedPatternDataInput::initAndConnectDriver(const char *pghost,
		const char *pgport, const char *dbName, const char *login,
		const char *pwd, TempDirCleaner * tempDirCleaner) {
	this->driver = makePtr<DBDriver>(tempDirCleaner);
	return this->driver->connect(pghost, pgport, dbName, login, pwd);
}

void SerializedPatternDataInput::reportDBDriverError() {
	cerr << this->driver->getMessage() << endl;
	LOG_F("ERROR")<< this->driver->getMessage();
}

bool SerializedPatternDataInput::loadDataFromDB(Ptr<vector<Ptr<ImageInfo>>> & data,
		QuickLZ* quickLZstate) {
	bool error = false;
	vector<int> userPatterns;
	if (!this->userID.empty()) {
		int uid = stoi(this->userID);
		userPatterns = this->driver->getUserPatterns(uid, error);
		if (error) {
			return false;
		}
	} else {
		if (this->patternsToFind.get() != NULL) {
			for (unsigned int i = 0; i < this->patternsToFind->size(); i++) {
				userPatterns.push_back((*patternsToFind)[i]);
			}
		} else {
			return (false);
		}
	}

	for (uint up = 0; up < userPatterns.size(); up++) {
		int pid = userPatterns.at(up);
		Ptr<DBPattern> dbp;
		bool patternError = false;
		if (!this->driver->retrievePattern(pid, patternError, true, dbp,
				this->tmpDir, quickLZstate)) {
			return false;
		}
		loadData(data, dbp->getData());
	}
	return true;
}

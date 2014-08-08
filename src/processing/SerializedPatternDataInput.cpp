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

namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;

SerializedPatternDataInput::SerializedPatternDataInput(std::string userID,
		const char *pghost, const char *pgport, const char *dbName,
		const char *login, const char *pwd, const string & tmpDir) {
	if (!initAndConnectDriver(pghost, pgport, dbName, login, pwd)) {
		reportDBDriverError();
		exit(EXIT_FAILURE);
	}
	this->tmpDir = tmpDir;
	this->userID = userID;
	this->cache = new vector<ImageInfo*>(0);
	this->loaded = false;
	this->current = -1;
	this->patternsToFind = NULL;
}

SerializedPatternDataInput::SerializedPatternDataInput(
		vector<int>* patternsToFind, const char *pghost, const char *pgport,
		const char *dbName, const char *login, const char *pwd, const string & tmpDir) {
	if (!initAndConnectDriver(pghost, pgport, dbName, login, pwd)) {
		reportDBDriverError();
		exit(EXIT_FAILURE);
	}
	this->tmpDir = tmpDir;
	this->patternsToFind = patternsToFind;
	this->cache = new vector<ImageInfo*>(0);
	this->loaded = false;
	this->current = -1;
}

SerializedPatternDataInput::~SerializedPatternDataInput() {
	if (driver != NULL) {
		delete driver;
	}
	if (this->cache != NULL) {
		std::for_each(cache->begin(), cache->end(),
				delete_pointer_element<ImageInfo*>());
		delete cache;
	}
}

bool SerializedPatternDataInput::nextInput(ImageInfo** output) {
	if (this->current < 0) {
		if (!this->loaded) {
			std::for_each(cache->begin(), cache->end(),
					delete_pointer_element<ImageInfo*>());
			this->cache->clear();
			if (!loadDataFromDB(this->cache)) {
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
		*output = this->cache->at(this->current);
		this->current++;
		return true;
	}
}

void SerializedPatternDataInput::reload() {
	this->current = -1;
}

//PRIVATE

void SerializedPatternDataInput::loadData(vector<ImageInfo*>* data,
		std::string * rawData) {
	std::string xmlData = "<?xml version=\"1.0\"?>";
	xmlData.append(*rawData);
	ImageInfo *ii = new ImageInfo();
	cv::FileStorage fstorage(xmlData.c_str(),
			cv::FileStorage::READ | cv::FileStorage::MEMORY);
	cv::FileNode n = fstorage.root();
	read(n, *ii);
	fstorage.release();
	data->push_back(ii);
}

void SerializedPatternDataInput::read(const cv::FileNode& node, ImageInfo& x,
		const ImageInfo& default_value) {
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

bool SerializedPatternDataInput::initAndConnectDriver(const char *pghost,
		const char *pgport, const char *dbName, const char *login,
		const char *pwd) {
	this->driver = new DBDriver();
	return this->driver->connect(pghost, pgport, dbName, login, pwd);
}

void SerializedPatternDataInput::reportDBDriverError() {
	std::cerr << this->driver->getMessage() << std::endl;
	LOG_F("ERROR")<< this->driver->getMessage();
}

bool SerializedPatternDataInput::loadDataFromDB(std::vector<ImageInfo*>* data) {
	bool error = false;
	std::vector<int> userPatterns;
	if (!this->userID.empty()) {
		int uid = std::stoi(this->userID);
		userPatterns = this->driver->getUserPatterns(uid, &error);
		if (error) {
			return false;
		}
	} else {
		if (this->patternsToFind != NULL) {
			for (unsigned int i = 0; i < this->patternsToFind->size(); i++) {
				userPatterns.push_back((*patternsToFind)[i]);
			}
		} else {
			return (false);
		}
	}

	for (uint up = 0; up < userPatterns.size(); up++) {
		int pid = userPatterns.at(up);
		DBPattern* dbp;
		bool patternError = false;
		if (!this->driver->retrievePattern(pid, &patternError, true, &dbp, this->tmpDir)) {
			return false;
		}
		loadData(data, dbp->getData());
	}
	return true;
}

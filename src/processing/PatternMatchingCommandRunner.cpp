/*
 * PatternMatchingCommandRunner.cpp
 *
 *  Created on: 20/05/2014
 *      Author: franco
 */

#include <processing/CommandRunner.hpp>
#include <processing/PatternMatchingCommandRunner.hpp>
#include "output/JSONValue.h"
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include "utils/Constants.hpp"
#include <logging/Log.hpp>

using namespace Anakin;

PatternMatchingCommandRunner::PatternMatchingCommandRunner(Flags* flags,
		DataOutput* out, SFBMCache* cache) :
				CommandRunner(flags, out, cache) {
}

PatternMatchingCommandRunner::~PatternMatchingCommandRunner(){
}

string PatternMatchingCommandRunner::getHelp() {
	return CommandRunner::getHelp();
}

void PatternMatchingCommandRunner::validateRequest(
		std::vector<std::string> *input) {
	inputError = false;
	action = 0;
	mma = 8;
	mr = 1.f / 1.5f;
	sceneID = -1;
	indexes.clear();
	reqID = "";
	lastError = "";
	if (flags->validateInput(input)) {
		std::vector<std::string>* values;
		if (flags->flagFound(Constants::ACTION_MATCH)) {
			action = PatternMatchingCommandRunner::MATCH;
		}
		if (flags->flagFound(Constants::ACTION_ADDIDX)) {
			action = PatternMatchingCommandRunner::ADDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_DELIDX)) {
			action = PatternMatchingCommandRunner::DELIDXS;
		}
		if (flags->flagFound(Constants::ACTION_UPDIDX)) {
			action = PatternMatchingCommandRunner::UPDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_STATUSIDX)) {
			action = PatternMatchingCommandRunner::IDXSTATUS;
		}
		if (flags->flagFound(Constants::PARAM_SCENEID)) {
			values = flags->getFlagValues(Constants::PARAM_SCENEID);
			if (values->size() != 1) {
				lastError = "flag " + Constants::PARAM_SCENEID
						+ " expects only one value";
				inputError = true;
				return;
			}
			sceneID = std::stoi(values->at(0));
		}
		if (flags->flagFound(Constants::PARAM_REQID)) {
			values = flags->getFlagValues(Constants::PARAM_REQID);
			if (values->size() != 1) {
				lastError = "flag " + Constants::PARAM_REQID
						+ " expects only one value";
				inputError = true;
				return;
			}
			reqID = values->at(0);
		}
		if (flags->flagFound(Constants::PARAM_MIN_RATIO)) {
			values = flags->getFlagValues(Constants::PARAM_MIN_RATIO);
			if (values->size() == 1) {
				this->mr = std::stof(values->at(0));
			} else {
				lastError = "flag " + Constants::PARAM_MIN_RATIO
						+ " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_MIN_MATCHES_ALLOWED)) {
			values = flags->getFlagValues(Constants::PARAM_MIN_MATCHES_ALLOWED);
			if (values->size() == 1) {
				this->mma = std::stoi(values->at(0));
			} else {
				lastError = "flag " + Constants::PARAM_MIN_MATCHES_ALLOWED
						+ " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_IDXS))   //MUST BE AT THE END
				{
			values = flags->getFlagValues(Constants::PARAM_IDXS);
			if (values->empty()) {
				lastError = "flag " + Constants::PARAM_IDXS
						+ " expects at least one value";
				inputError = true;
				return;
			}
			indexes.insert(indexes.begin(), values->begin(), values->end());
		}
	} else {
		lastError = "input error!";
		inputError = true;
	}
}

void PatternMatchingCommandRunner::run() {

	if (inputError) {
		this->out->error(
				this->rw->outputError(ResultWriter::RW_ERROR_TYPE_ERROR,
						lastError, "CommandRunner::run"));
		return;
	}

	int ireqID = std::stoi(reqID);

	switch (action) {
	case PatternMatchingCommandRunner::ADDIDXS: {
		std::vector<JSONValue*> inserts;
		std::string duplicated;
		if (!checkDuplicatedIndexes(this->indexes, &duplicated)) {
			this->out->error(
					this->rw->outputError(ResultWriter::RW_ERROR_TYPE_WARNING,
							"duplicated indexes on request: " + duplicated,
							"CommandRunner::run"));
			return;
		}
		for (uint i = 0; i < this->indexes.size(); i++) {
			int idxID = std::stoi(this->indexes.at(i));
			bool error = false;
			this->cache->loadMatcher(idxID, &error);
			if (error) {
				this->out->error(
						this->cache->getLastOperationResult()->Stringify().c_str());
				return;
			}
			inserts.push_back(this->cache->getLastOperationResult());
		}
		this->out->output(
				this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_ADD,
						inserts), ireqID);
		break;
	}
	case PatternMatchingCommandRunner::DELIDXS: {
		std::vector<JSONValue*> deletes;
		std::string duplicated;
		if (!checkDuplicatedIndexes(this->indexes, &duplicated)) {
			this->out->error(
					this->rw->outputError(ResultWriter::RW_ERROR_TYPE_WARNING,
							"duplicated indexes on request: " + duplicated,
							"CommandRunner::run"));
			return;
		}
		for (uint i = 0; i < this->indexes.size(); i++) {
			std::string smatcherID = this->indexes.at(i);
			int idxID = std::stoi(smatcherID);
			this->cache->unloadMatcher(idxID);
			deletes.push_back(this->cache->getLastOperationResult());
		}
		this->out->output(
				this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_DEL,
						deletes), ireqID);
		break;
	}
	case PatternMatchingCommandRunner::IDXSTATUS: {
		std::vector<JSONValue*> status;
		status.push_back(this->cache->indexCacheStatus());
		this->out->output(
				this->rw->outputResponse(reqID,
						ResultWriter::RW_CACHE_IDX_STATUS, status), ireqID);
		break;
	}
	case PatternMatchingCommandRunner::UPDIDXS: {
		std::vector<JSONValue*> updates;
		std::string duplicated;
		if (!checkDuplicatedIndexes(this->indexes, &duplicated)) {
			this->out->error(
					this->rw->outputError(ResultWriter::RW_ERROR_TYPE_WARNING,
							"duplicated indexes on request: " + duplicated,
							"CommandRunner::run"));
			return;
		}
		for (uint i = 0; i < this->indexes.size(); i++) {
			std::string smatcherID = this->indexes.at(i);
			int idxID = std::stoi(smatcherID);
			bool error = false;
			this->cache->updateMatcher(idxID, &error);
			if (error) {
				this->out->error(
						this->cache->getLastOperationResult()->Stringify().c_str());
				return;
			}
			updates.push_back(this->cache->getLastOperationResult());
		}
		this->out->output(
				this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_UPD,
						updates), ireqID);
		break;
	}
	case PatternMatchingCommandRunner::MATCH: {
		std::vector<JSONValue*>* matches = new std::vector<JSONValue*>();
		std::string duplicated;
		if (!checkDuplicatedIndexes(this->indexes, &duplicated)) {
			this->out->error(
					this->rw->outputError(ResultWriter::RW_ERROR_TYPE_WARNING,
							"duplicated indexes on request: " + duplicated,
							"CommandRunner::run"));
			return;
		}
		bool loadSceneError = false;
		ImageInfo* scene = this->cache->loadScene(sceneID, &loadSceneError);
		if (loadSceneError) {
			this->out->error(
					this->cache->getLastOperationResult()->Stringify().c_str());
			return;
		}
		RichImg* rscene = new RichImg(scene);
		for (uint i = 0; i < this->indexes.size(); i++) {
			std::string smatcherID = this->indexes.at(i);
			int idxID = std::stoi(smatcherID);
			bool matcherError = false;
			SerializableFlannBasedMatcher* matcher = this->cache->loadMatcher(
					idxID, &matcherError);
			if (matcherError) {
				this->out->error(
						this->cache->getLastOperationResult()->Stringify().c_str());
				return;
			}
			this->detector = new BasicFlannDetector(matcher, this->cache,
					this->mr, this->mma);
			this->processor = new FlannMatchingProcessor(this->detector,
					this->rw);
			bool processingError = false;
			std::vector<JSONValue*>* cmatches = this->processor->process(rscene,
					&processingError);
			if (processingError) {
				this->out->error(
						this->cache->getLastOperationResult()->Stringify().c_str());
				return;
			}
			matches->insert(matches->end(), cmatches->begin(), cmatches->end());
		}
		std::vector<JSONValue*> sceneMatches;
		sceneMatches.push_back(
				this->rw->matchesAsJSON(scene->getLabel(), *matches));
		this->out->output(
				this->rw->outputResponse(reqID,
						ResultWriter::RW_PATTERN_MATCHING, sceneMatches),
				ireqID);
		delete matches;
		break;
	}
	}
}


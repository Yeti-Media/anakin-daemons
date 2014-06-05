/*
 * PatternMatchingCommandRunner.cpp
 *
 *  Created on: 20/05/2014
 *      Author: Franco Pellegrini
 */

#include <processing/commandrunner/CommandRunner.hpp>
#include <processing/commandrunner/PatternMatchingCommandRunner.hpp>
#include "output/JSONValue.h"
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include "utils/Constants.hpp"
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <utils/help/HelpPatternMatching.hpp>

using namespace Anakin;

/**
 * static help initialization
 */
Help* PatternMatchingCommandRunner::help = new HelpPatternMatching();

PatternMatchingCommandRunner::PatternMatchingCommandRunner(DataOutput* out,
		SFBMCache* cache) :
		CommandRunner(out, cache) {
	flags->setMinCount(1);
	flags->setNoValuesFlag(Constants::ACTION_ADDIDX);
	flags->setNoValuesFlag(Constants::ACTION_DELIDX);
	flags->setNoValuesFlag(Constants::ACTION_UPDIDX);
	flags->setNoValuesFlag(Constants::ACTION_MATCH);
	flags->setOptionalFlag(Constants::PARAM_SCENEID);
	flags->setOptionalFlag(Constants::PARAM_REQID);
	flags->setOptionalFlag(Constants::PARAM_IDXS);
	flags->setOptionalFlag(Constants::ACTION_STATUSIDX);

	flags->setIncompatibility(Constants::ACTION_ADDIDX,
			Constants::ACTION_DELIDX);
	flags->setIncompatibility(Constants::ACTION_ADDIDX,
			Constants::ACTION_UPDIDX);
	flags->setIncompatibility(Constants::ACTION_ADDIDX,
			Constants::ACTION_STATUSIDX);
	flags->setIncompatibility(Constants::ACTION_DELIDX,
			Constants::ACTION_UPDIDX);

	flags->setDependence(Constants::ACTION_ADDIDX, Constants::PARAM_IDXS);
	flags->setDependence(Constants::ACTION_DELIDX, Constants::PARAM_IDXS);
	flags->setDependence(Constants::ACTION_UPDIDX, Constants::PARAM_IDXS);
	flags->setIncompatibility(Constants::ACTION_STATUSIDX,
			Constants::PARAM_IDXS);

	std::vector<std::string>* indexesLooseDependences = new std::vector<
			std::string>();
	indexesLooseDependences->push_back(Constants::ACTION_ADDIDX);
	indexesLooseDependences->push_back(Constants::ACTION_DELIDX);
	indexesLooseDependences->push_back(Constants::ACTION_UPDIDX);
	indexesLooseDependences->push_back(Constants::ACTION_MATCH);
	flags->setLooseDependencies(Constants::PARAM_IDXS, indexesLooseDependences);

	std::vector<std::string>* pmatchLooseDependences = new std::vector<
			std::string>();
	pmatchLooseDependences->push_back(Constants::PARAM_SCENEID);
	flags->setLooseDependencies(Constants::ACTION_MATCH,
			pmatchLooseDependences);
	flags->setDependence(Constants::ACTION_MATCH, Constants::PARAM_IDXS);

	flags->setIncompatibility(Constants::ACTION_ADDIDX,
			Constants::ACTION_MATCH);
	flags->setIncompatibility(Constants::ACTION_UPDIDX,
			Constants::ACTION_MATCH);
	flags->setIncompatibility(Constants::ACTION_DELIDX,
			Constants::ACTION_MATCH);
	flags->setIncompatibility(Constants::ACTION_STATUSIDX,
			Constants::ACTION_MATCH);

	std::vector<std::string>* reqIDLooseDependences = new std::vector<
			std::string>();
	reqIDLooseDependences->push_back(Constants::ACTION_MATCH);
	reqIDLooseDependences->push_back(Constants::ACTION_ADDIDX);
	reqIDLooseDependences->push_back(Constants::ACTION_DELIDX);
	reqIDLooseDependences->push_back(Constants::ACTION_UPDIDX);
	reqIDLooseDependences->push_back(Constants::ACTION_STATUSIDX);
	flags->setLooseDependencies(Constants::PARAM_REQID, reqIDLooseDependences);

	flags->setOptionalFlag(Constants::PARAM_MIN_RATIO);
	flags->setOptionalFlag(Constants::PARAM_MIN_MATCHES_ALLOWED);
	flags->setDependence(Constants::PARAM_MIN_RATIO, Constants::ACTION_MATCH);
	flags->setDependence(Constants::PARAM_MIN_MATCHES_ALLOWED,
			Constants::ACTION_MATCH);

	flags->setVerbose(true);
}

PatternMatchingCommandRunner::~PatternMatchingCommandRunner() {

}

void PatternMatchingCommandRunner::validateRequest(
		std::vector<std::string> *input) {
	inputError = false;
	action = NONE;
	mma = 8;
	mr = 1.f / 1.5f;
	sceneID = -1;
	indexes.clear();
	reqID = "";
	lastError = "";
	if (flags->validateInput(input)) {
		std::vector<std::string>* values;
		if (flags->flagFound(Constants::ACTION_MATCH)) {
			action = E_PatternMatchingAction::MATCH;
		}
		if (flags->flagFound(Constants::ACTION_ADDIDX)) {
			action = E_PatternMatchingAction::ADDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_DELIDX)) {
			action = E_PatternMatchingAction::DELIDXS;
		}
		if (flags->flagFound(Constants::ACTION_UPDIDX)) {
			action = E_PatternMatchingAction::UPDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_STATUSIDX)) {
			action = E_PatternMatchingAction::IDXSTATUS;
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
	case E_PatternMatchingAction::NONE: {
		LOG_F("ERROR")<< "You can't process NONE action in PatternMatchingCommandRunner::run()";
		break;
	}
	case E_PatternMatchingAction::ADDIDXS: {
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
	case E_PatternMatchingAction::DELIDXS: {
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
	case E_PatternMatchingAction::IDXSTATUS: {
		std::vector<JSONValue*> status;
		status.push_back(this->cache->indexCacheStatus());
		this->out->output(
				this->rw->outputResponse(reqID,
						ResultWriter::RW_CACHE_IDX_STATUS, status), ireqID);
		break;
	}
	case E_PatternMatchingAction::UPDIDXS: {
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
	case E_PatternMatchingAction::MATCH: {
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
			delete cmatches; //TODO review if this is WRONG
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


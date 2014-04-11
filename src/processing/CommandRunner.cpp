#include "processing/CommandRunner.hpp"
#include "output/JSONValue.h"
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include "utils/Constants.hpp"
#include <logging/Log.hpp>

using namespace Anakin;
using namespace cv;

CommandRunner::CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache, std::vector<std::string> *input) {
	this->out = out;
	this->rw = new ResultWriter();
	this->cache = cache;
	if (flags->validateInput(input)) {
		std::vector<std::string>* values;
		if (flags->flagFound(Constants::ACTION_MATCH)) {
			action = CommandRunner::MATCH;
		}
		if (flags->flagFound(Constants::ACTION_ADDIDX)) {
			action = CommandRunner::ADDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_DELIDX)) {
			action = CommandRunner::DELIDXS;
		}
		if (flags->flagFound(Constants::ACTION_UPDIDX)) {
			action = CommandRunner::UPDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_STATUSIDX)) {
			action = CommandRunner::IDXSTATUS;
		}
		if (flags->flagFound(Constants::PARAM_SCENEID)) {
			values = flags->getFlagValues(Constants::PARAM_SCENEID);
			if (values->size() != 1) {
				error = "flag " + Constants::PARAM_SCENEID + " expects only one value";
				inputError = true;
				return;
			}
			sceneID = std::stoi(values->at(0));
		}
		if (flags->flagFound(Constants::PARAM_REQID)) {
			values = flags->getFlagValues(Constants::PARAM_REQID);
			if (values->size() != 1) {
				error = "flag " + Constants::PARAM_REQID + " expects only one value";
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
				error = "flag " + Constants::PARAM_MIN_RATIO + " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_MIN_MATCHES_ALLOWED)) {
			values = flags->getFlagValues(Constants::PARAM_MIN_MATCHES_ALLOWED);
			if (values->size() == 1) {
				this->mma = std::stoi(values->at(0));
			} else {
				error = "flag " + Constants::PARAM_MIN_MATCHES_ALLOWED + " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_IDXS))   //MUST BE AT THE END
				{
			values = flags->getFlagValues(Constants::PARAM_IDXS);
			if (values->empty()) {
				error = "flag " + Constants::PARAM_IDXS + " expects at least one value";
				inputError = true;
				return;
			}
			indexes.insert(indexes.begin(), values->begin(), values->end());
		}
	} else {
		error = "input error!";
		inputError = true;
	}
}

CommandRunner::CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache) {
	this->flags = flags;
	this->rw = new ResultWriter();
	this->cache = cache;
	this->out = out;
}

void CommandRunner::validateRequest(std::vector<std::string> *input) {
	inputError = false;
	action = 0;
	mma = 8;
	mr = 1.f / 1.5f;
	sceneID = -1;
	indexes.clear();
	reqID = "";
	error = "";
	if (flags->validateInput(input)) {
		std::vector<std::string>* values;
		if (flags->flagFound(Constants::ACTION_MATCH)) {
			action = CommandRunner::MATCH;
		}
		if (flags->flagFound(Constants::ACTION_ADDIDX)) {
			action = CommandRunner::ADDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_DELIDX)) {
			action = CommandRunner::DELIDXS;
		}
		if (flags->flagFound(Constants::ACTION_UPDIDX)) {
			action = CommandRunner::UPDIDXS;
		}
		if (flags->flagFound(Constants::ACTION_STATUSIDX)) {
			action = CommandRunner::IDXSTATUS;
		}
		if (flags->flagFound(Constants::PARAM_SCENEID)) {
			values = flags->getFlagValues(Constants::PARAM_SCENEID);
			if (values->size() != 1) {
				error = "flag " + Constants::PARAM_SCENEID + " expects only one value";
				inputError = true;
				return;
			}
			sceneID = std::stoi(values->at(0));
		}
		if (flags->flagFound(Constants::PARAM_REQID)) {
			values = flags->getFlagValues(Constants::PARAM_REQID);
			if (values->size() != 1) {
				error = "flag " + Constants::PARAM_REQID + " expects only one value";
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
				error = "flag " + Constants::PARAM_MIN_RATIO + " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_MIN_MATCHES_ALLOWED)) {
			values = flags->getFlagValues(Constants::PARAM_MIN_MATCHES_ALLOWED);
			if (values->size() == 1) {
				this->mma = std::stoi(values->at(0));
			} else {
				error = "flag " + Constants::PARAM_MIN_MATCHES_ALLOWED + " expects only one value";
				inputError = true;
				return;
			}
		}
		if (flags->flagFound(Constants::PARAM_IDXS))   //MUST BE AT THE END
				{
			values = flags->getFlagValues(Constants::PARAM_IDXS);
			if (values->empty()) {
				error = "flag " + Constants::PARAM_IDXS + " expects at least one value";
				inputError = true;
				return;
			}
			indexes.insert(indexes.begin(), values->begin(), values->end());
		}
	} else {
		error = "input error!";
		inputError = true;
	}
}

int CommandRunner::run() {

	if (inputError) {
		this->out->error(this->rw->outputError(ResultWriter::RW_ERROR_TYPE_ERROR, error, "CommandRunner::run"));
		return -1;
	}

	int ireqID = std::stoi(reqID);

	switch (action) {
        case CommandRunner::ADDIDXS: {
            std::vector<JSONValue*> inserts;
            for (uint i = 0; i < this->indexes.size(); i++) {
                int idxID = std::stoi(this->indexes.at(i));
                bool error;
                this->cache->loadMatcher(idxID, &error);
                if (error) {
                    std::cout << "ADDIDXS error" << std::endl;
                    this->out->error(this->cache->getLastOperationResult()->Stringify().c_str());
                    return -1;
                }
                inserts.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_ADD, inserts), ireqID);
            break;
        }
        case CommandRunner::DELIDXS: {
            std::vector<JSONValue*> deletes;
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                this->cache->unloadMatcher(idxID);
                deletes.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_DEL, deletes), ireqID);
            break;
        }
        case CommandRunner::IDXSTATUS: {
            std::vector<JSONValue*> status;
            status.push_back(this->cache->indexCacheStatus());
            this->out->output(this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_STATUS, status), ireqID);
            break;
        }
        case CommandRunner::UPDIDXS: {
            std::vector<JSONValue*> updates;
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                bool error;
                this->cache->updateMatcher(idxID, &error);
                if (error) {
                    this->out->error(this->cache->getLastOperationResult()->Stringify().c_str());
                    return -1;
                }
                updates.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResponse(reqID, ResultWriter::RW_CACHE_IDX_UPD, updates), ireqID);
            break;
        }
        case CommandRunner::MATCH: {
            std::vector<JSONValue*>* matches = new std::vector<JSONValue*>();
            bool loadSceneError;
            ImageInfo* scene = this->cache->loadScene(sceneID, &loadSceneError);
            if (loadSceneError) {
                this->out->error(this->cache->getLastOperationResult()->Stringify().c_str());
                return -1;
            }
            RichImg* rscene = new RichImg(scene);
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                bool matcherError;
                SerializableFlannBasedMatcher* matcher = this->cache->loadMatcher(idxID, &matcherError);
                if (matcherError) {
                    this->out->error(this->cache->getLastOperationResult()->Stringify().c_str());
                    return -1;
                }
                this->detector = new BasicFlannDetector(matcher, this->cache, this->mr, this->mma);
                this->processor = new FlannMatchingProcessor(this->detector, this->rw);
                bool processingError;
                std::vector<JSONValue*>* cmatches = this->processor->process(rscene, &processingError);
                if (processingError) {
                    this->out->error(this->cache->getLastOperationResult()->Stringify().c_str());
                    return -1;
                }
                matches->insert(matches->end(), cmatches->begin(), cmatches->end());
            }
            std::vector<JSONValue*> sceneMatches;
            sceneMatches.push_back(this->rw->matchesAsJSON(scene->getLabel(), *matches));
            this->out->output(this->rw->outputResponse(reqID, ResultWriter::RW_PATTERN_MATCHING, sceneMatches), ireqID);
            delete matches;
            break;
        }
	}

	return 0;
}

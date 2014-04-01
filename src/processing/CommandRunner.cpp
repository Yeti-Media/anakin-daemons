#include "processing/CommandRunner.hpp"
#include "output/JSONValue.h"
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator
#include "utils/Constants.hpp"

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
            if (flags->flagFound(Constants::PARAM_IDXS)) { //MUST BE AT THE END
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
        this->out->output(error);
        return -1;
    }

    int ireqID = std::stoi(reqID);

    switch (action) {
        case CommandRunner::ADDIDXS : {
            std::vector<JSONValue*> inserts;
            for (uint i = 0; i < this->indexes.size(); i++) {
                int idxID = std::stoi(this->indexes.at(i));
                this->cache->loadMatcher(idxID);
                inserts.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_ADD, inserts), ireqID);
            break;
        }
        case CommandRunner::DELIDXS : {
            std::vector<JSONValue*> deletes;
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                this->cache->unloadMatcher(idxID);
                deletes.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_DEL, deletes), ireqID);
            break;
        }
        case CommandRunner::IDXSTATUS : {
            std::vector<JSONValue*> status;
            status.push_back(this->cache->indexCacheStatus());
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_STATUS, status), ireqID);
            break;
        }
        case CommandRunner::UPDIDXS : {
            std::vector<JSONValue*> updates;
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                this->cache->updateMatcher(idxID);
                updates.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_UPD, updates), ireqID);
            break;
        }
        case CommandRunner::MATCH : {
            std::vector<JSONValue*>* matches = new std::vector<JSONValue*>();
            ImageInfo* scene = this->cache->loadScene(sceneID);
            RichImg* rscene = new RichImg(scene);
            for (uint i = 0; i < this->indexes.size(); i++) {
                std::string smatcherID = this->indexes.at(i);
                int idxID = std::stoi(smatcherID);
                SerializableFlannBasedMatcher* matcher = this->cache->loadMatcher(idxID);
                if (this->processor == NULL) {
                    this->detector = new BasicFlannDetector(matcher, this->cache);
                    this->processor = new FlannMatchingProcessor(this->detector, this->rw);
                } else {
                    this->detector->changeMatcher(matcher);
                }
                std::vector<JSONValue*>* cmatches = this->processor->process(rscene);
                matches->insert(matches->end(), cmatches->begin(), cmatches->end());
            }
            std::vector<JSONValue*> sceneMatches;
            sceneMatches.push_back(this->rw->resultAsJSONValue(scene->getLabel(), *matches));
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_PATTERN_MATCHING, sceneMatches), ireqID);
            delete matches;
            break;
        }
    }

    return 0;
}

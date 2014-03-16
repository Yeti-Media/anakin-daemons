#include "CommandRunner.hpp"
#include "JSONValue.h"
#include <algorithm> // for copy
#include <iterator> // for ostream_iterator

using namespace Anakin;
using namespace cv;

CommandRunner::CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache, std::vector<std::string> *input) {
    this->out = out;
    this->rw = new ResultWriter();
    this->cache = cache;
    if (flags->validateInput(input)) {
            std::vector<std::string>* values;
            if (flags->flagFound("pmatch")) {
                action = CommandRunner::MATCH;
            }
            if (flags->flagFound("addIndexes")) {
                action = CommandRunner::ADDIDXS;
            }
            if (flags->flagFound("remIndexes")) {
                action = CommandRunner::DELIDXS;
            }
            if (flags->flagFound("updateIndexes")) {
                action = CommandRunner::UPDIDXS;
            }
            if (flags->flagFound("cacheStatus")) {
                action = CommandRunner::IDXSTATUS;
            }
            if (flags->flagFound("sceneID")) {
                values = flags->getFlagValues("sceneID");
                if (values->size() != 1) {
                    error = "flag sceneID expects only one value";
                    inputError = true;
                    return;
                }
                sceneID = std::stoi(values->at(0));
            }
            if (flags->flagFound("reqID")) {
                values = flags->getFlagValues("reqID");
                if (values->size() != 1) {
                    error = "flag reqID expects only one value";
                    inputError = true;
                    return;
                }
                reqID = values->at(0);
            }
            if (flags->flagFound("indexes")) { //MUST BE AT THE END
                values = flags->getFlagValues("indexes");
                if (values->empty()) {
                    error = "flag indexes expects at least one value";
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

    switch (action) {
        case CommandRunner::ADDIDXS : {
            std::vector<JSONValue*> inserts;
            for (uint i = 0; i < this->indexes.size(); i++) {
                int idxID = std::stoi(this->indexes.at(i));
                this->cache->loadMatcher(idxID);
                inserts.push_back(this->cache->getLastOperationResult());
            }
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_ADD, inserts));
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
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_DEL, deletes));
            break;
        }
        case CommandRunner::IDXSTATUS : {
            std::vector<JSONValue*> status;
            status.push_back(this->cache->indexCacheStatus());
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_STATUS, status));
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
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_CACHE_IDX_UPD, updates));
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
            this->out->output(this->rw->outputResult(reqID, ResultWriter::RW_PATTERN_MATCHING, sceneMatches));
            delete matches;
            break;
        }
    }

    return 0;
}

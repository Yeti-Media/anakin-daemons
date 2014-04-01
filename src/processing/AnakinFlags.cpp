#include "processing/AnakinFlags.hpp"
#include "utils/Constants.hpp"

using namespace Anakin;

AnakinFlags::AnakinFlags() {}

Flags* AnakinFlags::getFlags(bool verbose) {
    Flags* flags = new Flags();
        flags->setMinCount(1);
        flags->setNoValuesFlag(Constants::ACTION_ADDIDX);
        flags->setNoValuesFlag(Constants::ACTION_DELIDX);
        flags->setNoValuesFlag(Constants::ACTION_UPDIDX);
        flags->setNoValuesFlag(Constants::ACTION_MATCH);
        flags->setOptionalFlag(Constants::PARAM_SCENEID);
        flags->setOptionalFlag(Constants::PARAM_REQID);
        flags->setOptionalFlag(Constants::PARAM_IDXS);
        flags->setOptionalFlag(Constants::ACTION_STATUSIDX);

        flags->setIncompatibility(Constants::ACTION_ADDIDX, Constants::ACTION_DELIDX);
        flags->setIncompatibility(Constants::ACTION_ADDIDX, Constants::ACTION_UPDIDX);
        flags->setIncompatibility(Constants::ACTION_ADDIDX, Constants::ACTION_STATUSIDX);
        flags->setIncompatibility(Constants::ACTION_DELIDX, Constants::ACTION_UPDIDX);

        flags->setDependence(Constants::ACTION_ADDIDX, Constants::PARAM_IDXS);
        flags->setDependence(Constants::ACTION_DELIDX, Constants::PARAM_IDXS);
        flags->setDependence(Constants::ACTION_UPDIDX, Constants::PARAM_IDXS);
        flags->setIncompatibility(Constants::ACTION_STATUSIDX, Constants::PARAM_IDXS);

        std::vector<std::string>* indexesLooseDependences = new std::vector<std::string>();
        indexesLooseDependences->push_back(Constants::ACTION_ADDIDX);
        indexesLooseDependences->push_back(Constants::ACTION_DELIDX);
        indexesLooseDependences->push_back(Constants::ACTION_UPDIDX);
        indexesLooseDependences->push_back(Constants::ACTION_MATCH);
        flags->setLooseDependencies(Constants::PARAM_IDXS, indexesLooseDependences);

        std::vector<std::string>* pmatchLooseDependences = new std::vector<std::string>();
        pmatchLooseDependences->push_back(Constants::PARAM_SCENEID);
        flags->setLooseDependencies(Constants::ACTION_MATCH, pmatchLooseDependences);
        flags->setDependence(Constants::ACTION_MATCH, Constants::PARAM_IDXS);

        flags->setIncompatibility(Constants::ACTION_ADDIDX, Constants::ACTION_MATCH);
        flags->setIncompatibility(Constants::ACTION_UPDIDX, Constants::ACTION_MATCH);
        flags->setIncompatibility(Constants::ACTION_DELIDX, Constants::ACTION_MATCH);
        flags->setIncompatibility(Constants::ACTION_STATUSIDX, Constants::ACTION_MATCH);

        std::vector<std::string>* reqIDLooseDependences = new std::vector<std::string>();
        reqIDLooseDependences->push_back(Constants::ACTION_MATCH);
        reqIDLooseDependences->push_back(Constants::ACTION_ADDIDX);
        reqIDLooseDependences->push_back(Constants::ACTION_DELIDX);
        reqIDLooseDependences->push_back(Constants::ACTION_UPDIDX);
        reqIDLooseDependences->push_back(Constants::ACTION_STATUSIDX);
        flags->setLooseDependencies(Constants::PARAM_REQID, reqIDLooseDependences);

        flags->setVerbose(verbose);
        return flags;
}

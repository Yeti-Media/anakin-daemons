#include "AnakinFlags.hpp"

using namespace Anakin;

AnakinFlags::AnakinFlags() {}

Flags* AnakinFlags::getFlags(bool verbose) {
    Flags* flags = new Flags();
        flags->setMinCount(1);
        flags->setNoValuesFlag("addIndexes");
        flags->setNoValuesFlag("remIndexes");
        flags->setNoValuesFlag("updateIndexes");
        flags->setNoValuesFlag("pmatch");
        flags->setOptionalFlag("sceneID");
        flags->setOptionalFlag("scene");
        flags->setOptionalFlag("reqID");
        flags->setOptionalFlag("indexes");
        flags->setOverridingFlag("cacheStatus");

        flags->setIncompatibility("addIndexes", "remIndexes");
        flags->setIncompatibility("addIndexes", "updateIndexes");
        flags->setIncompatibility("remIndexes", "updateIndexes");

        flags->setDependence("addIndexes", "indexes");
        flags->setDependence("remIndexes", "indexes");
        flags->setDependence("updateIndexes", "indexes");

        std::vector<std::string>* indexesLooseDependences = new std::vector<std::string>();
        indexesLooseDependences->push_back("addIndexes");
        indexesLooseDependences->push_back("remIndexes");
        indexesLooseDependences->push_back("updateIndexes");
        indexesLooseDependences->push_back("pmatch");
        flags->setLooseDependencies("indexes", indexesLooseDependences);

        std::vector<std::string>* pmatchLooseDependences = new std::vector<std::string>();
        pmatchLooseDependences->push_back("sceneID");
        pmatchLooseDependences->push_back("scene");
        flags->setLooseDependencies("pmatch", pmatchLooseDependences);
        flags->setDependence("pmatch", "indexes");

        flags->setIncompatibility("addIndexes", "pmatch");
        flags->setIncompatibility("updateIndexes", "pmatch");
        flags->setIncompatibility("remIndexes", "pmatch");

        std::vector<std::string>* reqIDLooseDependences = new std::vector<std::string>();
        reqIDLooseDependences->push_back("pmatch");
        reqIDLooseDependences->push_back("addIndexes");
        reqIDLooseDependences->push_back("remIndexes");
        reqIDLooseDependences->push_back("updateIndexes");
        flags->setLooseDependencies("reqID", reqIDLooseDependences);

        flags->setVerbose(verbose);
        return flags;
}

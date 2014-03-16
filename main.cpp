#include "DBDriver.hpp"
#include "DBUser.hpp"
#include "DBPattern.hpp"
#include "DBHistogram.hpp"
#include "Flags.hpp"
#include "XMLoader.hpp"
#include "Constants.hpp"
#include "ImageInfo.hpp"
#include <iostream>
#include "Help.hpp"

using namespace Anakin;

void showHelp() {
    Help* h = new Help();
    std::cout << h->getFullHelp() << std::endl;
}

int main(int argc, const char * argv[]) {
    std::string path;
    bool saveObjects=false;
    int userID;
    bool saveUser=false;
    bool showhelp = false;
    char objectsAs = 0;
    bool load = false;
    std::string smatcher_id;
    bool savePatterns = false;
    int sceneID = -1;
    bool loadingScenes = false;


//    const char * argv_[] = {
//    };
//    int argc_ = 0;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }

    Flags* flags = new Flags();
    flags->setMinCount(1);
    flags->setOverridingFlag("help");
    flags->setOptionalFlag("user");
    flags->setOptionalFlag("path");
    flags->setNoValuesFlag("patterns");
    flags->setNoValuesFlag("scenes");
    flags->setNoValuesFlag("histograms");
    flags->setNoValuesFlag("landscapes");
    flags->setOptionalFlag("index");
    flags->setNoValuesFlag("savePatterns");
    vector<string>* pathLooseDeps = new vector<string>(0);
    pathLooseDeps->push_back("patterns");
    pathLooseDeps->push_back("histograms");
    pathLooseDeps->push_back("landscapes");
    pathLooseDeps->push_back("savePatterns");
    pathLooseDeps->push_back("scenes");
    flags->setLooseDependencies("path", pathLooseDeps);
    flags->setIncompatibility("index", "path");
    flags->setIncompatibility("patterns", "histograms");
    flags->setIncompatibility("patterns", "landscapes");
    flags->setIncompatibility("patterns", "scenes");
    flags->setIncompatibility("histograms", "landscapes");
    flags->setIncompatibility("histograms", "scenes");
    flags->setIncompatibility("landscapes", "scenes");
    flags->setIncompatibility("index", "patterns");
    flags->setIncompatibility("index", "histograms");
    flags->setIncompatibility("index", "landscapes");
    flags->setIncompatibility("index", "scenes");
    flags->setNoValuesFlag("load");
    vector<string>* loadLooseDeps = new vector<string>(0);
    loadLooseDeps->push_back("user");
    loadLooseDeps->push_back("index");
    loadLooseDeps->push_back("patterns");
    loadLooseDeps->push_back("histograms");
    loadLooseDeps->push_back("landscapes");
    loadLooseDeps->push_back("scenes");
    flags->setLooseDependencies("load", loadLooseDeps);
    flags->setIncompatibility("load", "path");
    flags->setDependence("savePatterns", "index");
    flags->setIncompatibility("savePatterns", "load");
    flags->setOptionalFlag("sceneID");
    flags->setDependence("sceneID", "load");
    flags->setDependence("sceneID", "scenes");


    if (flags->validateInput(input)) {
        if (flags->flagFound("help")) {
            showhelp = true;
        }
        std::vector<std::string>* values;
        if (flags->flagFound("user")) {
            saveUser = true;
            values = flags->getFlagValues("user");
            if (values->size() == 1) {
                userID = std::stoi(values->at(0));
            } else {
                std::cerr << "param user need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("path")) {
            saveObjects = true;
            values = flags->getFlagValues("path");
            if (values->size() == 1) {
                path = values->at(0);
            } else {
                std::cerr << "param path need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("patterns")) {
            objectsAs = Constants::PATTERN;
        }
        if (flags->flagFound("histograms")) {
            objectsAs = Constants::HISTOGRAM;
        }
        if (flags->flagFound("landscapes")) {
            objectsAs = Constants::LANDSCAPE;
        }
        if (flags->flagFound("index")) {
            objectsAs = Constants::INDEX;
        }
        if (flags->flagFound("scenes")) {
            objectsAs = Constants::SCENE;
        }
        if (flags->flagFound("load")) {
            load = true;
            if (objectsAs == Constants::SCENE) {
                loadingScenes = true;
            }
        }
        if (flags->flagFound("sceneID")) {
            values = flags->getFlagValues("sceneID");
            if (values->size() == 1) {
                sceneID = std::stoi(values->at(0));
            } else {
                std::cerr << "param sceneID need only one value\n";
                return -1;
            }
        }
        if (flags->flagFound("savePatterns")) {
            savePatterns = true;
        }

        if (flags->flagFound("index")) {
            saveObjects = true;
            values = flags->getFlagValues("index");
            if (values->size() == 1 && load) {
                smatcher_id = values->at(0);
            } else if (values->size() == 2 && !load) {
                smatcher_id = values->at(0);
                userID = std::stoi(values->at(1));
            } else {
                std::cerr << "param index need two values when saving and one value when loading\n";
                return -1;
            }
        }

        if (loadingScenes && sceneID == -1) { //THIS MUST BE AT THE END
            std::cerr << "Missing sceneID flag!" << std::endl;
            return -1;
        }
    } else {
        std::cerr << "Input error!" << std::endl;
        return -1;
    }

    if (showhelp) {
        showHelp();
        return 0;
    }

    DBDriver* driver = new DBDriver();
    driver->connect();
    std::cout << driver->lastMessageReceived << std::endl;

    DBUser* user;
    std::vector<DBPattern*>* patterns;
    std::vector<DBHistogram*>* histograms;
    std::vector<DBHistogram*>* landscapes;
    if (load) {
        if (objectsAs != Constants::INDEX && objectsAs != Constants::SCENE) user = new DBUser(userID);
        bool error = false;
        switch (objectsAs) {
            case Constants::PATTERN : {
                std::vector<int> pattern_ids = driver->getUserPatterns(user->getID(), &error);
                if (error) {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                for (uint i = 0; i < pattern_ids.size(); i++) {
                    DBPattern* pattern;
                    if (!driver->retrievePattern(pattern_ids.at(i), true, &pattern)) {
                        std::cerr << driver->lastMessageReceived << std::endl;
                        return -1;
                    }
                    //std::cout << "loaded pattern with label : " << pattern->getLabel() << " and id : " << pattern->getID() << std::endl;
                }
                std::cout << "user " << user->getID() << " have " << pattern_ids.size() << " patterns" << std::endl;
                break;
            }
            case Constants::HISTOGRAM : {
                std::vector<int> histogram_ids = driver->getUserHistograms(user->getID(), &error);
                if (error) {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                for (uint i = 0; i < histogram_ids.size(); i++) {
                    DBHistogram* histogram;
                    if (!driver->retrieveHistogram(histogram_ids.at(i), true, &histogram)) {
                        std::cerr << driver->lastMessageReceived << std::endl;
                        return -1;
                    }
                    std::cout << "loaded histogram : " << histogram->getID() << std::endl;
                }
                break;
            }
            case Constants::LANDSCAPE : {
                std::vector<int> landscape_ids = driver->getUserLandscapes(user->getID(), &error);
                if (error) {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                for (uint i = 0; i < landscape_ids.size(); i++) {
                    DBHistogram* landscape;
                    if (!driver->retrieveHistogram(landscape_ids.at(i), true, &landscape)) {
                        std::cerr << driver->lastMessageReceived << std::endl;
                        return -1;
                    }
                    std::cout << "loaded landscape : " << landscape->getID() << std::endl;
                }
                break;
            }
            case Constants::INDEX : {
                int trainerID = std::stoi(smatcher_id);
                if (driver->retrieveSFBM(trainerID)) {
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                break;
            }
            case Constants::SCENE : {
                ImageInfo* scene;
                if (driver->retrieveScene(&scene, sceneID)) {
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                break;
            }
        }
        return 0;
    }
    if (saveUser) {
        user = new DBUser(userID);
    }
    if (saveObjects) {
        XMLoader* loader;
        if (objectsAs != Constants::INDEX) loader = new XMLoader(path);
        switch (objectsAs) {
            case Constants::PATTERN : {
                patterns = loader->loadAsPattern();
                if (saveUser) {
                    for (uint p = 0; p < patterns->size(); p++) {
                        user->addPattern(patterns->at(p));
                    }
                    driver->saveUserPatterns(user, true);
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    for (uint p = 0; p < patterns->size(); p++) {
                        driver->savePattern(patterns->at(p));
                        std::cout << driver->lastMessageReceived << std::endl;
                    }
                }
                break;
            }
            case Constants::HISTOGRAM : {
                histograms = loader->loadAsHistogram();
                if (saveUser) {
                    for (uint p = 0; p < histograms->size(); p++) {
                        user->addHistogram(histograms->at(p));
                    }
                    driver->saveUserHistograms(user, true);
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    for (uint p = 0; p < histograms->size(); p++) {
                        driver->saveHORL(histograms->at(p), true);
                        std::cout << driver->lastMessageReceived << std::endl;
                    }
                }
                break;
            }
            case Constants::LANDSCAPE : {
                landscapes = loader->loadAsLandscape();
                if (saveUser) {
                    for (uint p = 0; p < landscapes->size(); p++) {
                        user->addLandscape(landscapes->at(p));
                    }
                    driver->saveUserLandscapes(user, true);
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    for (uint p = 0; p < landscapes->size(); p++) {
                        driver->saveHORL(landscapes->at(p), true);
                        std::cout << driver->lastMessageReceived << std::endl;
                    }
                }
                break;
            }
            case Constants::INDEX : {
                int trainer_id;
                if (driver->storeSFBM(smatcher_id, &trainer_id, userID, true, false)) {
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
                if (savePatterns) {
                    bool error;
                    std::vector<int> patterns = driver->getUserPatterns(userID, &error);
                    if (error) {
                        std::cerr << driver->lastMessageReceived << std::endl;
                        return -1;
                    }
                    for (uint p = 0; p < patterns.size(); p++) {
                        if (driver->storeNthPattern(trainer_id, p, patterns.at(p))) {
                            std::cout << driver->lastMessageReceived << std::endl;
                        } else {
                            std::cerr << driver->lastMessageReceived << std::endl;
                        }
                    }
                }
                break;
            }
            case Constants::SCENE : {
                patterns = loader->loadAsPattern();
                for (uint s = 0; s < patterns->size(); s++) {
                    DBPattern* sceneAsDBPattern = patterns->at(s);
                    if (driver->storeScene(sceneAsDBPattern)) {
                        std::cout << driver->lastMessageReceived << std::endl;
                    } else {
                        std::cerr << driver->lastMessageReceived << std::endl;
                        return -1;
                    }
                }
                break;
            }
        }
    } else {
        driver->saveUser(user);
        std::cout << driver->lastMessageReceived << std::endl;
    }

    return 0;
}

#include "DBDriver.hpp"
#include "DBUser.hpp"
#include "DBPattern.hpp"
#include "DBHistogram.hpp"
#include "Flags.hpp"
#include "XMLoader.hpp"
#include "Constants.hpp"
#include <iostream>

using namespace Anakin;

void showHelp() {
    std::cout << "dbconnector help" << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "./dbtest -help" << std::endl;
    std::cout << "./dbtest -user <ID> (-path <value> (-patterns|-landscapes|-histograms))" << std::endl;
    std::cout << "./dbtest -index <smatcher_id> [-load]" << std::endl;
    std::cout << std::endl;
    std::cout << "-index <smatcher_id>: for loading there must be two files called <smatcher_id>.xml and <smatcher_id>.if" << std::endl;
    std::cout << "                      for saving two files will be created, <smatcher_id>.xml and <smatcher_id>.if" << std::endl;
    std::cout << "-user <ID>: the user to save, if a path is also provided then this user will be related to the objects in that path" << std::endl;
    std::cout << "-path <value>: the path to the objects (patterns, landscapes o histograms) it must be a folder except for patterns where it can be a file" << std::endl;
    std::cout << "-patterns: the objects found at the path will be treated as patterns" << std::endl;
    std::cout << "-landscapes: the objects found at the path will be treated as landscapes" << std::endl;
    std::cout << "-histograms: the objects found at the path will be treated as histograms" << std::endl;
    std::cout << std::endl;
    std::cout << "examples:" << std::endl;
    std::cout << "./dbtest -user uibdybhiuyr3y3" << std::endl;
    std::cout << "./dbtest -path google.xml -patterns" << std::endl;
    std::cout << "./dbtest -path horls/ -landscapes" << std::endl;
    std::cout << "    the folder structure inside the folder passed with the path flag when loading landscapes or histograms must be the following:" << std::endl;
    std::cout << "        root /" << std::endl;
    std::cout << "            pattern / (1)" << std::endl;
    std::cout << "                color / (2)" << std::endl;
    std::cout << "                gray  /" << std::endl;
    std::cout << "                hsv   /" << std::endl;
    std::cout << "            landscape / (3)" << std::endl;
    std::cout << "                color /" << std::endl;
    std::cout << "                gray  /" << std::endl;
    std::cout << "                hsv   /" << std::endl;
    std::cout << std::endl;
    std::cout << "(1) : subfolder where histograms are stored" << std::endl;
    std::cout << "(2) : subsubfolder where color version of histograms are stored" << std::endl;
    std::cout << "(3) : subfolder where landscapes are stored" << std::endl;
}

int main(int argc, const char * argv[]) {
    std::string path;
    bool saveObjects=false;
    std::string userID;
    bool saveUser=false;
    bool showhelp = false;
    char objectsAs = 0;
    bool load = false;
    std::string smatcher_id;


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
    flags->setNoValuesFlag("histograms");
    flags->setNoValuesFlag("landscapes");
    flags->setOptionalFlag("index");
    vector<string>* pathLooseDeps = new vector<string>(0);
    pathLooseDeps->push_back("patterns");
    pathLooseDeps->push_back("histograms");
    pathLooseDeps->push_back("landscapes");
    flags->setLooseDependencies("path", pathLooseDeps);
    flags->setIncompatibility("index", "path");
    flags->setIncompatibility("patterns", "histograms");
    flags->setIncompatibility("patterns", "landscapes");
    flags->setIncompatibility("histograms", "landscapes");
    flags->setIncompatibility("index", "patterns");
    flags->setIncompatibility("index", "histograms");
    flags->setIncompatibility("index", "landscapes");
    flags->setNoValuesFlag("load");
    vector<string>* loadLooseDeps = new vector<string>(0);
    loadLooseDeps->push_back("user");
    loadLooseDeps->push_back("index");
    loadLooseDeps->push_back("patterns");
    loadLooseDeps->push_back("histograms");
    loadLooseDeps->push_back("landscapes");
    flags->setLooseDependencies("load", loadLooseDeps);
    flags->setIncompatibility("load", "path");



    if (flags->validateInput(input)) {
        if (flags->flagFound("help")) {
            showhelp = true;
        }
        std::vector<std::string>* values;
        if (flags->flagFound("user")) {
            saveUser = true;
            values = flags->getFlagValues("user");
            if (values->size() == 1) {
                userID = values->at(0);
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
        if (flags->flagFound("index")) {
            saveObjects = true;
            values = flags->getFlagValues("index");
            if (values->size() == 1) {
                smatcher_id = values->at(0);
            } else {
                std::cerr << "param index need only one value\n";
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
        if (flags->flagFound("load")) {
            load = true;
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
        if (objectsAs != Constants::INDEX) user = new DBUser(userID);
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
                    std::cout << "loaded histogram with label : " << histogram->getLabel() << " and id : " << histogram->getID() << std::endl;
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
                    std::cout << "loaded landscape with label : " << landscape->getLabel() << " and id : " << landscape->getID() << std::endl;
                }
                break;
            }
            case Constants::INDEX : {
                if (driver->retrieveSFBM(smatcher_id)) {
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
                    int id;
                    for (uint p = 0; p < patterns->size(); p++) {
                        driver->savePattern(patterns->at(p), &id);
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
                    int id;
                    for (uint p = 0; p < histograms->size(); p++) {
                        driver->saveHORL(histograms->at(p), &id);
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
                    int id;
                    for (uint p = 0; p < landscapes->size(); p++) {
                        driver->saveHORL(landscapes->at(p), &id);
                        std::cout << driver->lastMessageReceived << std::endl;
                    }
                }
                break;
            }
            case Constants::INDEX : {
                if (driver->storeSFBM(smatcher_id)) {
                    std::cout << driver->lastMessageReceived << std::endl;
                } else {
                    std::cerr << driver->lastMessageReceived << std::endl;
                    return -1;
                }
            }
        }
    } else {
        driver->saveUser(user);
        std::cout << driver->lastMessageReceived << std::endl;
    }

    return 0;
}

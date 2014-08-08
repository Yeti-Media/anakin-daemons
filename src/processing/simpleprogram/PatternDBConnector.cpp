/*
 * PatternDBConnector.cpp
 *
 *  Created on: 08/06/2014
 *      Author: Franco Pellegrini
 */

#include <db/DBDriver.hpp>
#include <db/DBHistogram.hpp>
#include <db/DBPattern.hpp>
#include <db/DBUser.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <processing/Flags.hpp>
#include <processing/simpleprogram/PatternDBConnector.hpp>
#include <sys/types.h>
#include <utils/ClearVector.hpp>
#include <utils/Constants.hpp>
#include <utils/help/HelpPatternDBConnector.hpp>
#include <utils/XMLoader.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;

namespace Anakin {

PatternDBConnector::PatternDBConnector() :
		Program() {
}

PatternDBConnector::~PatternDBConnector() {
}

Help* PatternDBConnector::getHelp() {
	return new HelpPatternDBConnector();
}

string PatternDBConnector::getProgramName() {
	return "PatternDBConnector";
}

void PatternDBConnector::initProgramFlags() {
	this->programFlags.setOptionalFlag("user");
	this->programFlags.setOptionalFlag("path");
	this->programFlags.setNoValuesFlag("patterns");
	this->programFlags.setNoValuesFlag("scenes");
	this->programFlags.setNoValuesFlag("histograms");
	this->programFlags.setNoValuesFlag("landscapes");
	this->programFlags.setOptionalFlag("index");
	this->programFlags.setNoValuesFlag("savePatterns");

	vector<string> pathLooseDeps(0);
	pathLooseDeps.push_back("patterns");
	pathLooseDeps.push_back("histograms");
	pathLooseDeps.push_back("landscapes");
	pathLooseDeps.push_back("savePatterns");
	pathLooseDeps.push_back("scenes");
	this->programFlags.setLooseDependencies("path", &pathLooseDeps);
	this->programFlags.setIncompatibility("index", "path");
	this->programFlags.setIncompatibility("patterns", "histograms");
	this->programFlags.setIncompatibility("patterns", "landscapes");
	this->programFlags.setIncompatibility("patterns", "scenes");
	this->programFlags.setIncompatibility("histograms", "landscapes");
	this->programFlags.setIncompatibility("histograms", "scenes");
	this->programFlags.setIncompatibility("landscapes", "scenes");
	this->programFlags.setIncompatibility("index", "patterns");
	this->programFlags.setIncompatibility("index", "histograms");
	this->programFlags.setIncompatibility("index", "landscapes");
	this->programFlags.setIncompatibility("index", "scenes");
	this->programFlags.setNoValuesFlag("load");
	vector<string> loadLooseDeps(0);
	loadLooseDeps.push_back("user");
	loadLooseDeps.push_back("index");
	loadLooseDeps.push_back("patterns");
	loadLooseDeps.push_back("histograms");
	loadLooseDeps.push_back("landscapes");
	loadLooseDeps.push_back("scenes");
	this->programFlags.setLooseDependencies("load", &loadLooseDeps);
	this->programFlags.setIncompatibility("load", "path");
	this->programFlags.setDependence("savePatterns", "index");
	this->programFlags.setIncompatibility("savePatterns", "load");
	this->programFlags.setOptionalFlag("sceneID");
	this->programFlags.setDependence("sceneID", "load");
	this->programFlags.setDependence("sceneID", "scenes");
}

int PatternDBConnector::run(vector<string> *input) {
	string path;
	bool saveObjects = false;
	int userID;
	bool saveUser = false;
	char objectsAs = 0;
	bool load = false;
	string smatcher_id;
	bool savePatterns = false;
	int sceneID = -1;
	bool loadingScenes = false;

	vector<string>* values = NULL;

	if (this->programFlags.flagFound("user")) {
		saveUser = true;
		values = this->programFlags.getFlagValues("user");
		if (values->size() == 1) {
			userID = stoi(values->at(0));
		} else {
			cerr << "param user need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags.flagFound("path")) {
		saveObjects = true;
		values = this->programFlags.getFlagValues("path");
		if (values->size() == 1) {
			path = values->at(0);
		} else {
			cerr << "param path need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags.flagFound("patterns")) {
		objectsAs = Constants::PATTERN;
	}
	if (this->programFlags.flagFound("histograms")) {
		objectsAs = Constants::HISTOGRAM;
	}
	if (this->programFlags.flagFound("landscapes")) {
		objectsAs = Constants::LANDSCAPE;
	}
	if (this->programFlags.flagFound("index")) {
		objectsAs = Constants::INDEX;
	}
	if (this->programFlags.flagFound("scenes")) {
		objectsAs = Constants::SCENE;
	}
	if (this->programFlags.flagFound("load")) {
		load = true;
		if (objectsAs == Constants::SCENE) {
			loadingScenes = true;
		}
	}
	if (this->programFlags.flagFound("sceneID")) {
		values = this->programFlags.getFlagValues("sceneID");
		if (values->size() == 1) {
			sceneID = stoi(values->at(0));
		} else {
			cerr << "param sceneID need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags.flagFound("savePatterns")) {
		savePatterns = true;
	}

	if (this->programFlags.flagFound("index")) {
		saveObjects = true;
		values = this->programFlags.getFlagValues("index");
		if (values->size() == 1 && load) {
			smatcher_id = values->at(0);
		} else if (values->size() == 2 && !load) {
			smatcher_id = values->at(0);
			userID = stoi(values->at(1));
		} else {
			cerr
					<< "param index need two values when saving and one value when loading"
					<< endl;
			return EXIT_FAILURE;
		}
	}

	if (loadingScenes && sceneID == -1)   //THIS MUST BE AT THE END
			{
		cerr << "Missing sceneID flag!" << endl;
		return EXIT_FAILURE;
	}

	DBDriver* driver = new DBDriver();
	driver->connect("", "", "", "", "");
	cout << driver->getMessage() << endl;
	LOG_F("Info")<< driver->getMessage();

	DBUser* user = NULL;
	if (load) {
		if (objectsAs != Constants::INDEX && objectsAs != Constants::SCENE)
			user = new DBUser(userID);
		bool error = false;
		switch (objectsAs) {
		case Constants::PATTERN: {
			vector<int> pattern_ids = driver->getUserPatterns(user->getID(),
					&error);
			if (error) {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR")<< driver->getMessage();
				return EXIT_FAILURE;
			}
			for (uint i = 0; i < pattern_ids.size(); i++) {
				bool patternError = false;
				DBPattern* pattern;
				if (!driver->retrievePattern(pattern_ids.at(i), &patternError,
						true, &pattern, this->tempDir)) {
					cerr << driver->getMessage() << endl;
					LOG_F("ERROR")<< driver->getMessage();
					return EXIT_FAILURE;
				}
				//cout << "loaded pattern with label : " << pattern->getLabel() << " and id : " << pattern->getID() << endl;
			}
			cout << "user " << user->getID() << " have " << pattern_ids.size()
					<< " patterns" << endl;
			LOG_F("Info")<< "user " << user->getID() << " have "
			<< pattern_ids.size() << " patterns";
			break;
		}
		case Constants::HISTOGRAM: {
			vector<int> histogram_ids = driver->getUserHistograms(user->getID(),
					&error);
			if (error) {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR")<< driver->getMessage();
				return EXIT_FAILURE;
			}
			for (uint i = 0; i < histogram_ids.size(); i++) {
				bool histogramError = false;
				DBHistogram* histogram;
				if (!driver->retrieveHistogram(histogram_ids.at(i),
						&histogramError, true, &histogram, this->tempDir)) {
					cerr << driver->getMessage() << endl;
					LOG_F("ERROR")<< driver->getMessage();
					return EXIT_FAILURE;
				}
				cout << "loaded histogram : " << histogram->getID() << endl;
				LOG_F("Info")<< "loaded histogram : " << histogram->getID();
			}
			break;
		}
		case Constants::LANDSCAPE: {
			vector<int> landscape_ids = driver->getUserLandscapes(user->getID(),
					&error);
			if (error) {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR")<< driver->getMessage();
				return EXIT_FAILURE;
			}
			for (uint i = 0; i < landscape_ids.size(); i++) {
				bool histogramError = false;
				DBHistogram* landscape;
				if (!driver->retrieveLandscape(landscape_ids.at(i),
						&histogramError, true, &landscape, this->tempDir)) {
					cerr << driver->getMessage() << endl;
					LOG_F("ERROR")<< driver->getMessage();
					return EXIT_FAILURE;
				}
				cout << "loaded landscape : " << landscape->getID() << endl;
				LOG_F("Info")<<"loaded landscape : " << landscape->getID();
			}
			break;
		}
		case Constants::INDEX: {
			int trainerID = stoi(smatcher_id);
			bool SFBMError = false;
			if (driver->retrieveSFBM(trainerID, &SFBMError, this->tempDir)) {
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR") << driver->getMessage();
				return EXIT_FAILURE;
			}
			break;
		}
		case Constants::SCENE: {
			ImageInfo* scene;
			bool sceneError = false;
			if (driver->retrieveScene(&scene, sceneID, &sceneError)) {
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR") << driver->getMessage();
				return EXIT_FAILURE;
			}
			break;
		}
		}
		//FIXME refactor this exit and the other
		delete driver;
		if (user != NULL) {
			delete user;
		}
		return EXIT_SUCCESS;
	}
	if (saveUser) {
		user = new DBUser(userID);
	}
	if (saveObjects) {
		XMLoader* loader = NULL;

		if (objectsAs != Constants::INDEX)
			loader = new XMLoader(path);
		switch (objectsAs) {
		case Constants::PATTERN: {
			vector<DBPattern*>* patterns = loader->loadAsPattern(true);
			if (saveUser) {
				for (uint p = 0; p < patterns->size(); p++) {
					user->addPattern(patterns->at(p));
				}
				driver->saveUserPatterns(user,this->tempDir, true);
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < patterns->size(); p++) {
					driver->savePattern(patterns->at(p));
					cout << driver->getMessage() << endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			delete patterns;
			break;
		}
		case Constants::HISTOGRAM: {
			vector<DBHistogram*>* histograms = loader->loadAsHistogram(true);
			if (saveUser) {
				for (uint p = 0; p < histograms->size(); p++) {
					user->addHistogram(histograms->at(p));
				}
				driver->saveUserHistograms(user,this->tempDir, true);
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < histograms->size(); p++) {
					driver->saveHORL(histograms->at(p),this->tempDir, true);
					cout << driver->getMessage() << endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			delete histograms;
			break;
		}
		case Constants::LANDSCAPE: {
			vector<DBHistogram*>* landscapes = loader->loadAsLandscape(true);
			if (saveUser) {
				for (uint p = 0; p < landscapes->size(); p++) {
					user->addLandscape(landscapes->at(p));
				}
				driver->saveUserLandscapes(user,this->tempDir, true);
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				for (uint p = 0; p < landscapes->size(); p++) {
					driver->saveHORL(landscapes->at(p),this->tempDir, true);
					cout << driver->getMessage() << endl;
					LOG_F("Info") << driver->getMessage();
				}
			}
			delete landscapes;
			break;
		}
		case Constants::INDEX: {
			int trainer_id;
			if (driver->storeSFBM(smatcher_id, &trainer_id, userID,this->tempDir, true)) {
				cout << driver->getMessage() << endl;
				LOG_F("Info")<< driver->getMessage();
			} else {
				cerr << driver->getMessage() << endl;
				LOG_F("ERROR") << driver->getMessage();
				return EXIT_FAILURE;
			}
			if (savePatterns) {
				bool error;
				vector<int> patterns = driver->getUserPatterns(userID, &error);
				if (error) {
					cerr << driver->getMessage() << endl;
					LOG_F("ERROR")<< driver->getMessage();
					return EXIT_FAILURE;
				}
				for (uint p = 0; p < patterns.size(); p++) {
					if (driver->storeNthPattern(trainer_id, p,
							patterns.at(p))) {
						cout << driver->getMessage() << endl;
						LOG_F("Info")<< driver->getMessage();
					} else {
						cerr << driver->getMessage() << endl;
						LOG_F("ERROR") << driver->getMessage();
					}
				}
			}
			break;
		}
		case Constants::SCENE: {
			vector<DBPattern*>* patterns = loader->loadAsPattern(true);
			for (uint s = 0; s < patterns->size(); s++) {
				DBPattern* sceneAsDBPattern = patterns->at(s);
				if (driver->storeScene(sceneAsDBPattern)) {
					cout << driver->getMessage() << endl;
					LOG_F("Info")<< driver->getMessage();
				} else {
					cerr << driver->getMessage() << endl;
					LOG_F("ERROR") << driver->getMessage();
					return EXIT_FAILURE;
				}
			}
			delete patterns;
			break;
		}
		}

		if (loader != NULL) {
			delete loader;
		}
	} else {
		driver->saveUser(user);
		cout << driver->getMessage() << endl;
		LOG_F("Info")<<driver->getMessage();
	}

	delete driver;

	if (user != NULL) {
		delete user;
	}
	return EXIT_SUCCESS;
}

}
/* namespace Anakin */

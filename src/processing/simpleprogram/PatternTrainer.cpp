/*
 * PatternTrainer.cpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */
#include <opencv2/core.hpp>
#include <data/PatternLoader.hpp>
#include <data/RichImg.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <opencv2/core/operations.hpp>
#include <opencv2/flann/miniflann.hpp>
#include <processing/BasicFlannTrainer.hpp>
#include <processing/Flags.hpp>
#include <processing/simpleprogram/PatternTrainer.hpp>
#include <processing/SerializedPatternDataInput.hpp>
#include <utils/help/HelpPatternTrainer.hpp>
#include <cstdlib>
#include <iostream>               // for cout
#include <utils/ClearVector.hpp>
#include <utils/files/TempDirCleaner.hpp>

using namespace std;
using namespace cv;

namespace Anakin {

PatternTrainer::PatternTrainer() :
		Program("PatternTrainer") {
	quickLZstate = new QuickLZ();
}

PatternTrainer::~PatternTrainer() {
	delete quickLZstate;
}

Ptr<Help> PatternTrainer::getHelp() {
	return makePtr<HelpPatternTrainer>();
}

void PatternTrainer::initProgramFlags() {
	this->programFlags->setOptionalFlag("user");
	this->programFlags->setOptionalFlag("patternsId");
	this->programFlags->setOptionalFlag("saveToFile");
}

int PatternTrainer::run(const Ptr<vector<string>> &input) {
	string userID;
	//char mode = 0;
	string folder;
	string fileName;
	Ptr<vector<int>> patternsId;
	bool user = false;

	Ptr<vector<string>> values;

	if (this->programFlags->flagFound("user")) {
		values = this->programFlags->getFlagValues("user");
		user = true;
		if (values->size() == 1) {
			userID = values->at(0);
		} else {
			cerr << "flag user needs only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("patternsId")) {
		values = this->programFlags->getFlagValues("patternsId");
		if (!values->empty()) {
			patternsId = makePtr<vector<int>>();
			for (unsigned int i = 0; i < values->size(); i++) {
				(*patternsId).push_back(stoi(values->at(i)));
			}
		} else {
			cerr << "flag patternsId needs at least one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("saveToFile")) {
		values = this->programFlags->getFlagValues("saveToFile");
		if (values->size() == 1) {
			folder = "";
			fileName = values->at(0);
		} else if (values->size() == 2) {
			folder = values->at(0);
			fileName = values->at(1);
		} else {
			cerr << "flag saveToFile needs between one and two values" << endl;
			return EXIT_FAILURE;
		}
	}

	TempDirCleaner tempDirCleaner(0);

	//TRAINING
	Ptr<flann::IndexParams> indexParams = makePtr<flann::KDTreeIndexParams>(4);
	Ptr<flann::SearchParams> searchParams = makePtr<flann::SearchParams>();
	Ptr<SerializableFlannBasedMatcher> matcher = makePtr<
			SerializableFlannBasedMatcher>(indexParams, searchParams,
			&tempDirCleaner);
	matcher->clear();
	Ptr<vector<Ptr<RichImg>>> patterns = makePtr<vector<Ptr<RichImg>>>();
	Ptr<SerializedPatternDataInput> sinput;
	if (user) {
		sinput = makePtr<SerializedPatternDataInput>(userID, "", "", "", "", "",
				this->tempDir, &tempDirCleaner);
	} else {
		sinput = makePtr<SerializedPatternDataInput>(patternsId, "", "", "", "",
				"", this->tempDir, &tempDirCleaner);
	}
	Ptr<PatternLoader> loader = makePtr<PatternLoader>(sinput, patterns);
	loader->load(quickLZstate);

	Ptr<Trainer> trainer = makePtr<BasicFlannTrainer>(matcher, patterns, folder,
			fileName);
	trainer->train_and_save(quickLZstate);

	return EXIT_SUCCESS;
}

} /* namespace Anakin */

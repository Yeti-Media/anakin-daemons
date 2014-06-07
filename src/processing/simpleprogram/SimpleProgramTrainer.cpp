/*
 * SimpleProgramTrainer.cpp
 *
 *  Created on: 06/06/2014
 *      Author: Franco Pellegrini
 */

#include <CompileConfigurations.hpp>
#include <data/PatternLoader.hpp>
#include <data/RichImg.hpp>
#include <matching/SerializableFlannBasedMatcher.hpp>
#include <opencv2/core/core.hpp>
//#include <opencv2/opencv.hpp>
#include <processing/BasicFlannTrainer.hpp>
#include <processing/Flags.hpp>
#include <processing/simpleprogram/SimpleProgramTrainer.hpp>
#include <processing/SerializedPatternDataInput.hpp>
#include <utils/help/HelpTrainer.hpp>
//#include "opencv2/features2d/features2d.hpp"
//#include "opencv2/nonfree/nonfree.hpp"
//#include "processing/Trainer.hpp"
#include <cstdlib>
#include <iostream>               // for cout
//#include <string>

using namespace std;
using namespace cv;

namespace Anakin {

SimpleProgramTrainer::SimpleProgramTrainer() :
		SimpleProgram() {
}

SimpleProgramTrainer::~SimpleProgramTrainer() {
}

Help* SimpleProgramTrainer::getHelp() {
	return new HelpTrainer();
}

void SimpleProgramTrainer::setupProgramFlags() {
	this->programFlags->setOptionalFlag("user");
	//this->programFlags->setNoValuesFlag("patterns");
	this->programFlags->setOptionalFlag("patternsId");
	this->programFlags->setOptionalFlag("saveToFile");
}

void SimpleProgramTrainer::excecute(vector<string> *input) {
	string userID;
	//char mode = 0;
	string folder;
	string fileName;
	vector<int>* patternsId;
	bool user = false;

	vector<string>* values = new vector<string>();

	if (this->programFlags->flagFound("user")) {
		values->clear();
		values = this->programFlags->getFlagValues("user");
		user = true;
		if (values->size() == 1) {
			userID = values->at(0);
		} else {
			cerr << "flag user needs only one value" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (this->programFlags->flagFound("patternsId")) {
		values->clear();
		values = this->programFlags->getFlagValues("patternsId");
		if (!values->empty()) {
			patternsId = new vector<int>(0);
			for (unsigned int i = 0; i < values->size(); i++) {
				(*patternsId).push_back(stoi(values->at(i)));
			}
		} else {
			cerr << "flag patternsId needs at least one value" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (this->programFlags->flagFound("saveToFile")) {
		values->clear();
		values = this->programFlags->getFlagValues("saveToFile");
		if (values->size() == 1) {
			folder = "";
			fileName = values->at(0);
		} else if (values->size() == 2) {
			folder = values->at(0);
			fileName = values->at(1);
		} else {
			cerr << "flag saveToFile needs between one and two values" << endl;
			exit(EXIT_FAILURE);
		}
	}
	delete values;

	//TRAINING
	const Ptr<flann::IndexParams>& indexParams = new flann::KDTreeIndexParams(
			4);
	const Ptr<flann::SearchParams>& searchParams = new flann::SearchParams();
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::Ptr<FlannBasedMatcher>(
			new SerializableFlannBasedMatcher(indexParams, searchParams));
	matcher->clear();
	vector<RichImg*> patterns;
	SerializedPatternDataInput* sinput;
	if (user) {
		sinput = new SerializedPatternDataInput(userID, "", "", "", "", "");
	} else {
		sinput = new SerializedPatternDataInput(patternsId, "", "", "", "", "");
	}
	PatternLoader* loader = new PatternLoader(sinput, patterns);
	loader->load();

	Trainer* trainer = new BasicFlannTrainer(matcher, patterns, folder,
			fileName);
	trainer->train_and_save();

	//FIXME verify this
//	if (patternsId != NULL) {
//		delete patternsId;
//	}

	if (trainer != NULL) {
		delete trainer;
	}
	if (loader != NULL) {
		delete loader;
	}
	if (sinput != NULL) {
		delete sinput;
	}

}

} /* namespace Anakin */

/*
 * PatternExtractor.cpp
 *
 *  Created on: 09/06/2014
 *      Author: Franco Pellegrini
 */

#include <data/HistogramComparator.hpp>
#include <data/HistogramsIO.hpp>
#include <data/ImageDataInput.hpp>
#include <data/PatternLoader.hpp>
#include <data/RichImg.hpp>
#include <data/SingleImageDataInput.hpp>
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <processing/Flags.hpp>
#include <processing/simpleprogram/PatternExtractor.hpp>
#include <utils/help/HelpPatternExtractor.hpp>
#include <algorithm>
#include <cstdlib>
#include <iostream>               // for cout

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

namespace Anakin {

PatternExtractor::PatternExtractor() :
		Program("PatternExtractor") {
	quickLZState = new QuickLZ();
}

PatternExtractor::~PatternExtractor() {
	delete quickLZState;
}

Ptr<Help> PatternExtractor::getHelp() {
	return makePtr<HelpPatternExtractor>();
}

void PatternExtractor::initProgramFlags() {
	//I/O MODE
	this->programFlags->setNoValuesFlag("landscape");
	this->programFlags->setNoValuesFlag("histograms");

	this->programFlags->setNoValuesFlag("matching");
	this->programFlags->setIncompatibility("landscape", "histograms");
	this->programFlags->setIncompatibility("landscape", "matching");
	this->programFlags->setIncompatibility("histograms", "matching");
	//I/O paths
	this->programFlags->setOptionalFlag("iFile");
	this->programFlags->setOptionalFlag("iFolder");
	this->programFlags->setIncompatibility("iFile", "iFolder");
	this->programFlags->setOptionalFlag("oPath");
	Ptr<vector<string>> oPathLooseDeps = makePtr<vector<string>>();
	oPathLooseDeps->push_back("iFile");
	oPathLooseDeps->push_back("iFolder");
	this->programFlags->setLooseDependencies("oPath", oPathLooseDeps);
	this->programFlags->setIncompatibility("landscape", "iFile");
	this->programFlags->setNoValuesFlag("toJson");
	this->programFlags->setIncompatibility("toJson", "oPath");
	Ptr<vector<string>> imodesLooseDeps = makePtr<vector<string>>();
	imodesLooseDeps->push_back("oPath");
	imodesLooseDeps->push_back("toJson");
	this->programFlags->setLooseDependencies("landscape", imodesLooseDeps);
	this->programFlags->setLooseDependencies("histograms", imodesLooseDeps);
	this->programFlags->setLooseDependencies("matching", imodesLooseDeps);

	//HISTOGRAMS/LANDSCAPE MODE
	this->programFlags->setNoValuesFlag("color");
	this->programFlags->setNoValuesFlag("hsv");
	this->programFlags->setNoValuesFlag("gray");
	Ptr<vector<string>> modesLooseDeps = makePtr<vector<string>>();
	modesLooseDeps->push_back("color");
	modesLooseDeps->push_back("hsv");
	modesLooseDeps->push_back("gray");
	this->programFlags->setLooseDependencies("landscape", modesLooseDeps);
	this->programFlags->setLooseDependencies("histograms", modesLooseDeps);
	this->programFlags->setIncompatibility("color", "matching");
	this->programFlags->setIncompatibility("gray", "matching");
	this->programFlags->setIncompatibility("hsv", "matching");
	//LANDSCAPE EXTRA OPTIONS
	this->programFlags->setOptionalFlag("label");
	this->programFlags->setDependence("label", "landscape");
	this->programFlags->setDependence("landscape", "label");
	//FORMAT OPTIONS
	this->programFlags->setNoValuesFlag("xml");
	this->programFlags->setNoValuesFlag("yml");
	this->programFlags->setIncompatibility("xml", "yml");
//	vector<string> formatLooseDeps(0);
//	formatLooseDeps.push_back("matching");
//	formatLooseDeps.push_back("landscape");
//	formatLooseDeps.push_back("histograms");
	//LOAD ON DEMAND
	this->programFlags->setNoValuesFlag("lod");
	//FIXME fix this forced verbose
	this->programFlags->setVerbose(true);
}

int PatternExtractor::run(const Ptr<vector<string>> & input) {
	bool useInputPathAsDir = false;
	char mode = 0;
	char inputMode = 0;
	string label = "";
	string inputDir = "";
	string outputDir = "";
	bool saveToFile = true;
	bool useYaml = true;
	bool loadOnDemand = false;

	Ptr<vector<string>> values;

	if (this->programFlags->flagFound("landscape")) {
		inputMode = LANDSCAPE;
	}
	if (this->programFlags->flagFound("histograms")) {
		inputMode = HISTOGRAMS;
	}
	if (this->programFlags->flagFound("matching")) {
		inputMode = PATTERNS;
	}
	if (this->programFlags->flagFound("iFile")) {
		useInputPathAsDir = false;
		values = this->programFlags->getFlagValues("iFile");
		if (values->size() == 1) {
			inputDir = values->at(0);
		} else {
			cerr << "param iFile need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("iFolder")) {
		useInputPathAsDir = true;
		values = this->programFlags->getFlagValues("iFolder");
		if (values->size() == 1) {
			inputDir = values->at(0);
		} else {
			cerr << "param iFolder need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("oPath")) {
		values = this->programFlags->getFlagValues("oPath");
		if (values->size() == 1) {
			outputDir = values->at(0);
		} else {
			cerr << "param oPath need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("label")) {
		values = this->programFlags->getFlagValues("label");
		if (values->size() == 1) {
			label = values->at(0);
		} else {
			cerr << "param label need only one value" << endl;
			return EXIT_FAILURE;
		}
	}
	if (this->programFlags->flagFound("color")) {
		mode = mode | COLOR;
	}
	if (this->programFlags->flagFound("gray")) {
		mode = mode | GRAY;
	}
	if (this->programFlags->flagFound("hsv")) {
		mode = mode | HSV;
	}
	if (this->programFlags->flagFound("toJson")) {
		saveToFile = false;
	}
	if (this->programFlags->flagFound("xml")) {
		useYaml = false;
	}
	if (this->programFlags->flagFound("yml")) {
		useYaml = true;
	}
	if (this->programFlags->flagFound("lod")) {
		loadOnDemand = true;
	}

	Ptr<FeatureDetector> fdetector;
	Ptr<DescriptorExtractor> dextractor;

	Ptr<vector<Ptr<RichImg>>> patterns = makePtr<vector<Ptr<RichImg>>>();
	Ptr<DataInput> patternsDataInput;
	Ptr<PatternLoader> patternsLoader;

	if (!useInputPathAsDir) {
		patternsDataInput = makePtr<SingleImageDataInput>(inputDir);
	} else {
		patternsDataInput = makePtr<ImageDataInput>(inputDir, loadOnDemand);
	}

	if (inputMode & PATTERNS) {
		fdetector = makePtr<SurfFeatureDetector>(400);
		dextractor = makePtr<SurfDescriptorExtractor>();
		patternsLoader = makePtr<PatternLoader>(patternsDataInput, patterns,
				fdetector, dextractor);
		char mode = useYaml ? PatternLoader::YAML : PatternLoader::XML;
		patternsLoader->load_and_save(outputDir, saveToFile, mode);
		return EXIT_SUCCESS;
	} else {
		patternsLoader = makePtr<PatternLoader>(patternsDataInput, patterns,
				fdetector, dextractor);
		patternsLoader->load(quickLZState);
	}

	Ptr<HistogramsIO> io = makePtr<HistogramsIO>(outputDir);
	HistogramComparator hComparator(patterns, io);
	if (useYaml) {
		mode = mode | HistogramComparator::YAML;
	} else {
		mode = mode | HistogramComparator::XML;
	}
	if (inputMode & HISTOGRAMS) {
		hComparator.makeAndSaveHistograms(mode, saveToFile);
	} else if (inputMode & LANDSCAPE) {
		hComparator.makeAndSaveLandscape(mode, label, saveToFile);
	}

	return EXIT_SUCCESS;
}

} /* namespace Anakin */

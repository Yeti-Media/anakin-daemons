#include "data/HistogramsIO.hpp"
#include "output/ResultWriter.hpp"
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;
using namespace cv;

HistogramsIO::HistogramsIO(string baseFolder) {
	this->baseFolder = baseFolder;
	this->colorHistograms = new vector<Histogram*>(0);
	this->grayHistograms = new vector<Histogram*>(0);
	this->hsvHistograms = new vector<Histogram*>(0);
}

HistogramsIO::~HistogramsIO() {
	cleanupHistogramVector(this->colorHistograms);
	cleanupHistogramVector(this->grayHistograms);
	cleanupHistogramVector(this->hsvHistograms);
}

void HistogramsIO::cleanupHistogramVector(vector<Histogram*>* hVector) {
	for (uint p = 0; p < hVector->size(); p++) {
		delete hVector->at(p);
	}
	delete hVector;
}

void HistogramsIO::load(const char mode) {
	string subfolder = mode & LANDSCAPE ? "landscape/" : "pattern/";
	if (mode & COLOR) {
		load(this->baseFolder + subfolder + "color/", this->colorHistograms);
	}
	if (mode & GRAY) {
		load(this->baseFolder + subfolder + "gray/", this->grayHistograms);
	}
	if (mode & HSV) {
		load(this->baseFolder + subfolder + "hsv/", this->hsvHistograms);
	}
}

void HistogramsIO::save(vector<Histogram*>* input, const char mode,
		bool saveToFile) {
	string subfolder = mode & LANDSCAPE ? "landscape/" : "pattern/";
	string subSubFolder =
			mode & COLOR ? "color/" : ((mode & GRAY) ? "gray/" : "hsv/");
	string extension = mode & YAML ? ".yml" : ".xml";
	for (uint i = 0; i < input->size(); i++) {
		Histogram* current = input->at(i);
		string filename =
				saveToFile ?
						(this->baseFolder + subfolder + subSubFolder
								+ current->getLabel() + extension) :
						("output" + extension);
		save(filename, current, saveToFile);
	}
}

vector<Histogram*>* HistogramsIO::getColorHistograms() {
	return this->colorHistograms;
}

vector<Histogram*>* HistogramsIO::getGrayHistograms() {
	return this->grayHistograms;
}

vector<Histogram*>* HistogramsIO::getHSVHistograms() {
	return this->hsvHistograms;
}

//PRIVATE

void HistogramsIO::load(string baseFolder, vector<Histogram*>* output) {
	loadData(output, baseFolder);
}

void HistogramsIO::save(string filename, Histogram* histogram,
		bool saveToFile) {
	FileStorage fs(filename,
			saveToFile ?
					FileStorage::WRITE :
					(FileStorage::WRITE | FileStorage::MEMORY));
	write(fs, "Histogram", *histogram);
	if (saveToFile) {
		fs.release();
	} else {
		string data = fs.releaseAndGetString();
		bool isLandscape = histogram->isMinMax() || histogram->hasAvg();
		bool color = histogram->getChannels() == 3;
		bool hsv = histogram->getChannels() == 2;
		wcout
				<< ResultWriter::output(
						isLandscape ?
								ResultWriter::RW_LANDSCAPE :
								ResultWriter::RW_HISTOGRAMS, data,
						color ? ResultWriter::RW_COLOR : (
								hsv ? ResultWriter::RW_HSV : ResultWriter::RW_GRAY))
				<< endl;
	}
}

void HistogramsIO::read(const cv::FileNode& node, Histogram& x,
		const Histogram& default_value) {
	if (node.empty())
		x = default_value;
	else
		x.read(node);
}

void HistogramsIO::write(FileStorage& fs, const string&, const Histogram& x) {
	x.write(fs);
}

void HistogramsIO::loadData(vector<Histogram*>* data, string folder) {
	data->clear();
	if (fs::exists(folder)) {
		fs::directory_iterator end_itr; // default construction yields past-the-end
		for (fs::directory_iterator itr(folder); itr != end_itr; ++itr) {

			if (!fs::is_directory(itr->status())) {
				Histogram *ii = new Histogram();
				cv::FileStorage fstorage(itr->path().c_str(),
						cv::FileStorage::READ);
				cv::FileNode n = fstorage.root();
				read(n, *ii);
				fstorage.release();
				data->push_back(ii);
			}

		}

	} else {
		cerr << "HistogramsIO#loadData : directory : " << folder
				<< " doesn't exist" << endl;
		LOG_F("ERROR")<< "HistogramsIO#loadData : directory : " << folder
		<< " doesn't exist";
		exit(EXIT_FAILURE);
	}
}

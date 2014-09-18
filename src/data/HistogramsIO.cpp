#include "data/HistogramsIO.hpp"
#include <output/communicationFormatter/ICommunicationFormatter.hpp>
#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
namespace fs = boost::filesystem;

using namespace Anakin;
using namespace std;
using namespace cv;

HistogramsIO::HistogramsIO(const string & baseFolder) {
	this->baseFolder = baseFolder;
	this->colorHistograms = makePtr<vector<Ptr<Histogram>>>();
	this->grayHistograms = makePtr<vector<Ptr<Histogram>>>();
	this->hsvHistograms = makePtr<vector<Ptr<Histogram>>>();
}

HistogramsIO::~HistogramsIO() {
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

void HistogramsIO::save(const Ptr<vector<Ptr<Histogram>>>&input, const char mode,
bool saveToFile) {
	string subfolder = mode & LANDSCAPE ? "landscape/" : "pattern/";
	string subSubFolder =
	mode & COLOR ? "color/" : ((mode & GRAY) ? "gray/" : "hsv/");
	string extension = mode & YAML ? ".yml" : ".xml";
	for (uint i = 0; i < input->size(); i++) {
		Ptr<Histogram> current = input->at(i);
		string filename =
		saveToFile ?
		(this->baseFolder + subfolder + subSubFolder
		+ current->getLabel() + extension) :
		("output" + extension);
		save(filename, current, saveToFile);
	}
}

Ptr<vector<Ptr<Histogram>>> HistogramsIO::getColorHistograms() {
	return this->colorHistograms;
}

Ptr<vector<Ptr<Histogram>>> HistogramsIO::getGrayHistograms() {
	return this->grayHistograms;
}

Ptr<vector<Ptr<Histogram>>> HistogramsIO::getHSVHistograms() {
	return this->hsvHistograms;
}

//PRIVATE

void HistogramsIO::load(const string & baseFolder, Ptr<vector<Ptr<Histogram>>>& output) {
	loadData(output, baseFolder);
}

void HistogramsIO::save(const string & filename, const Ptr<Histogram> histogram,
		bool saveToFile) {
	FileStorage fs(filename,
			saveToFile ?
					FileStorage::WRITE :
					(FileStorage::WRITE | FileStorage::MEMORY));
	write(fs, "Histogram", *histogram);
	if (saveToFile) {
		fs.release();
	} else {
		I_CommunicationFormatter* cf = new CommunicationFormatterJSON();
		string data = fs.releaseAndGetString();
		bool isLandscape = histogram->isMinMax() || histogram->hasAvg();
		bool color = histogram->getChannels() == 3;
		bool hsv = histogram->getChannels() == 2;
		wstring out =
				*cf->format(
						isLandscape ?
								I_CommunicationFormatter::e_mode::CF_LANDSCAPE :
								I_CommunicationFormatter::e_mode::CF_HISTOGRAMS,
						data,
						color ? I_CommunicationFormatter::e_color::CF_COLOR : (
								hsv ? I_CommunicationFormatter::e_color::CF_HSV : I_CommunicationFormatter::e_color::CF_GRAY));
		wcout << out << endl;
		delete cf;
	}
}

void HistogramsIO::read(const FileNode & node, Ptr<Histogram> & x) {
	if (!node.empty())
		x->read(node);
}

void HistogramsIO::write(FileStorage & fs, const string & t,
		const Histogram & x) {
	x.write(fs);
}

void HistogramsIO::loadData(Ptr<vector<Ptr<Histogram>>>& data, const string &folder) {
	data->clear();
	if (fs::exists(folder)) {
		fs::directory_iterator end_itr; // default construction yields past-the-end
		for (fs::directory_iterator itr(folder); itr != end_itr; ++itr) {

			if (!fs::is_directory(itr->status())) {
				Ptr<Histogram> ii = makePtr<Histogram>();
				cv::FileStorage fstorage(itr->path().c_str(),
						cv::FileStorage::READ);
				cv::FileNode n = fstorage.root();
				read(n, ii);
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

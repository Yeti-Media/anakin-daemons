#include <matching/SerializableFlannBasedMatcher.hpp>
#include <opencv2/core/operations.hpp>
#include <stdio.h>
#include <sys/types.h>
#include <utils/Files.hpp>
#include <fstream>      // ifstream
#include <iostream>               // for cout

using namespace Anakin;
using namespace cv;
using namespace std;

SerializableFlannBasedMatcher::~SerializableFlannBasedMatcher() {

}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
		const Ptr<flann::IndexParams>& indexParams,
		const Ptr<flann::SearchParams>& searchParams) :
		FlannBasedMatcher(indexParams, searchParams) {
	this->loadedFromFile = false;
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
		QuickLZ* quickLZstate, string filename, bool removeFileAfterLoad) {
	this->filename = filename;
	string xmlData;
	decompress(quickLZstate, true, xmlData);
	this->load(xmlData);
	if (removeFileAfterLoad) {
		string indexFile = filename + ".if";
		string matcherFile = filename + ".xml";
		remove(indexFile.c_str());
		remove(matcherFile.c_str());
	}
}

void SerializableFlannBasedMatcher::save(QuickLZ* quickLZstate, string filename,
		string * xmlData) {
	this->filename = filename;
	bool saveXMLtoFile = xmlData == NULL;

	string fbmData;
	cv::FileStorage fs(fbmData,
			cv::FileStorage::WRITE | cv::FileStorage::MEMORY);
	write(fs);
	fbmData = fs.releaseAndGetString();

	cv::FileStorage mfs(saveXMLtoFile ? (filename + ".xml") : "output.xml",
			saveXMLtoFile ?
					cv::FileStorage::WRITE :
					(cv::FileStorage::WRITE | cv::FileStorage::MEMORY));

	mfs << "fbmData" << fbmData;
	cv::DescriptorMatcher::DescriptorCollection* mdptr = &mergedDescriptors;
	mdptr++;
	mdptr =
			(cv::DescriptorMatcher::DescriptorCollection*) ((vector<int> *) mdptr
					- 1);
	vector<int>* startIdxsPtr = ((vector<int> *) mdptr);
	mfs << "mergedDescriptorsstartIdxs" << "[";
	for (uint i = 0; i < startIdxsPtr->size(); i++) {
		mfs << startIdxsPtr->at(i);
	}
	mfs << "]";

	mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((cv::Mat *) mdptr
			- 1);
	cv::Mat* mergedDescriptorsDescriptors = ((cv::Mat *) mdptr);
	mfs << "mergedDescriptorsDescriptors" << *mergedDescriptorsDescriptors;

	if (saveXMLtoFile) {
		mfs.release();
	} else {
		*xmlData = mfs.releaseAndGetString();
	}
	saveIndex();
	compress(quickLZstate, true);
}

bool SerializableFlannBasedMatcher::empty() const {
	if (this->loadedFromFile)
		return false;
	return trainDescCollection.empty();
}

void SerializableFlannBasedMatcher::train(vector<cv::Mat> * descriptors) {
	mergedDescriptors.set(*descriptors);
	//FIXME can cause memory leaks
	flannIndex = new flann::Index(mergedDescriptors.getDescriptors(),
			*indexParams);
}

void SerializableFlannBasedMatcher::setID(string id) {
	this->smatcher_id = id;
}

string SerializableFlannBasedMatcher::getID() {
	return this->smatcher_id;
}

//PRIVATE

void SerializableFlannBasedMatcher::load(const string& xmlData) {
	cv::FileStorage fs(xmlData,
			(cv::FileStorage::READ | cv::FileStorage::MEMORY));

	cv::FileNode root = fs.root();
	string sfbmData = root["fbmData"];
	cv::FileStorage fbmDatafs(sfbmData,
			cv::FileStorage::READ | cv::FileStorage::MEMORY);
	read(fbmDatafs.root());
	cv::DescriptorMatcher::DescriptorCollection* mdptr = &mergedDescriptors;
	mdptr++;
	mdptr =
			(cv::DescriptorMatcher::DescriptorCollection*) ((vector<int> *) mdptr
					- 1);
	vector<int>* startIdxsPtr = ((vector<int> *) mdptr);

	cv::FileNode mdsidxnd = root["mergedDescriptorsstartIdxs"];
	cv::FileNodeIterator it = mdsidxnd.begin(), it_end = mdsidxnd.end(); // Go through the node
	for (; it != it_end; ++it)
		startIdxsPtr->push_back((int) *it);
	mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((cv::Mat *) mdptr
			- 1);
	cv::Mat* mergedDescriptorsDescriptors = ((cv::Mat *) mdptr);
	root["mergedDescriptorsDescriptors"] >> *mergedDescriptorsDescriptors;
	addedDescCount = mergedDescriptorsDescriptors->rows;
	loadIndex(mergedDescriptorsDescriptors);
	this->loadedFromFile = true;
	//delete mergedDescriptorsDescriptors;
}

void SerializableFlannBasedMatcher::saveIndex() {
	string tmpFile = this->filename + ".if";
	flannIndex->save(tmpFile);
}

void SerializableFlannBasedMatcher::loadIndex(cv::Mat * data) {
	string tmpFile = this->filename + ".if";
	cv::flann::IndexParams* params = new cv::flann::SavedIndexParams(
			tmpFile.c_str());
	//FIXME can cause memory leaks
	flannIndex = new cv::flann::Index(*data, *params);
}

void SerializableFlannBasedMatcher::compress(QuickLZ* quickLZstate,
		bool removeOriginal) {
	string uncompressedIndexFileName = this->filename + ".if";
	string uncompressedMatcherFileName = this->filename + ".xml";
	string compressedIndexFileName =
			removeOriginal ?
					uncompressedIndexFileName : this->filename + ".cif";
	string compressedMatcherFileName =
			removeOriginal ?
					uncompressedMatcherFileName : this->filename + ".cxml";
	string * uindexData = get_file_contents(uncompressedIndexFileName);
	string * umatcherData = get_file_contents(uncompressedMatcherFileName);
	if (removeOriginal) {
		remove(uncompressedIndexFileName.c_str());
		remove(uncompressedMatcherFileName.c_str());
	}
	size_t ir;
	size_t mr;
	//clock_t t_1 = clock();

	char * compressedIndexData = quickLZstate->compressText(uindexData, &ir);
	char * compressedMatcherData = quickLZstate->compressText(umatcherData,
			&mr);

	//clock_t t_2 = clock();
	//float tt = ((float)(t_2 - t_1))/CLOCKS_PER_SEC;
	//cout << "compressing time: " << tt << endl;
	write_to_file(compressedIndexData, compressedIndexFileName, ir);
	write_to_file(compressedMatcherData, compressedMatcherFileName, mr);
	delete uindexData;
	delete umatcherData;
	delete compressedIndexData;
	delete compressedMatcherData;
}

void SerializableFlannBasedMatcher::decompress(QuickLZ* quickLZstate,
		bool useOriginalNames, string & xmlData) {
	//bool decompressMatcherToFile = xmlData == NULL;
	string uncompressedIndexFileName = this->filename
			+ (useOriginalNames ? ".if" : ".uif");
	string uncompressedMatcherFileName = this->filename
			+ (useOriginalNames ? ".xml" : ".uxml");
	string compressedIndexFileName = this->filename
			+ (useOriginalNames ? ".if" : ".cif");
	string compressedMatcherFileName = this->filename
			+ (useOriginalNames ? ".xml" : ".cxml");

	string * cindexData = get_file_contents(compressedIndexFileName);
	string * cmatcherData = get_file_contents(compressedMatcherFileName);

	if (useOriginalNames) {
		remove(compressedIndexFileName.c_str());
		remove(compressedMatcherFileName.c_str());
	}
	//clock_t t_1 = clock();
	size_t ir;
	size_t mr;
	char * ucompressedIndexData = quickLZstate->decompressText(cindexData, &ir);
	char * ucompressedMatcherData = quickLZstate->decompressText(cmatcherData,
			&mr);

	xmlData = string(ucompressedMatcherData, mr);
	//indexData = new string(ucompressedIndexData, ir);

//	//clock_t t_2 = clock();
//	//float tt = ((float)(t_2 - t_1))/CLOCKS_PER_SEC;
//	//cout << "decompressing time: " << tt << endl;
	write_to_file(ucompressedIndexData, uncompressedIndexFileName, ir);

	delete cindexData;
	delete cmatcherData;
	delete ucompressedIndexData;
	delete ucompressedMatcherData;
}


#include <matching/SerializableFlannBasedMatcher.hpp>
#include <opencv2/core/operations.hpp>
#include <stdio.h>
#include <sys/types.h>
#include <utils/files/Files.hpp>
#include <fstream>      // ifstream
#include <iostream>               // for cout

using namespace Anakin;
using namespace cv;
using namespace std;

SerializableFlannBasedMatcher::~SerializableFlannBasedMatcher() {
	//tempDirCleaner should be deleted at the caller method
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
		const Ptr<flann::IndexParams>& indexParams,
		const Ptr<flann::SearchParams>& searchParams,
		TempDirCleaner * tempDirCleaner) :
		FlannBasedMatcher(indexParams, searchParams), tempDirCleaner(
				tempDirCleaner), loadedFromFile(false) {
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
		QuickLZ* quickLZstate, string filename, const string & tmpDir,
		TempDirCleaner * tempDirCleaner) :
		filename(filename), tempDirCleaner(tempDirCleaner) {
	string * xmlData = new string();
	decompress_from_file(tmpDir + filename + ".xml", quickLZstate, xmlData);
	decompress_file(tmpDir + filename + ".if", quickLZstate);
	this->load(*xmlData, tmpDir);
	delete xmlData;

	tempDirCleaner->deleteFile(fs::path(tmpDir + filename + ".xml"));
	tempDirCleaner->deleteFile(fs::path(tmpDir + filename + ".if"));
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
	saveIndexAndXML(quickLZstate);
}

bool SerializableFlannBasedMatcher::empty() const {
	if (this->loadedFromFile)
		return false;
	return trainDescCollection.empty();
}

void SerializableFlannBasedMatcher::train(vector<cv::Mat> * descriptors) {
	mergedDescriptors.set(*descriptors);
	//FIXME can cause memory leaks
	flannIndex = makePtr<flann::Index>(mergedDescriptors.getDescriptors(),
			*indexParams);
}

void SerializableFlannBasedMatcher::setID(string id) {
	this->smatcher_id = id;
}

string SerializableFlannBasedMatcher::getID() {
	return this->smatcher_id;
}

//PRIVATE

void SerializableFlannBasedMatcher::load(const string& xmlData,
		const string & tmpDir) {
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
	loadIndex(mergedDescriptorsDescriptors, tmpDir);
	this->loadedFromFile = true;
	//delete mergedDescriptorsDescriptors;
}

void SerializableFlannBasedMatcher::saveIndexAndXML(QuickLZ* quickLZstate) {
	string tmpFile = this->filename + ".if";
	flannIndex->save(tmpFile);
	compress_file(tmpFile, quickLZstate);
	tmpFile = this->filename + ".xml";
	compress_file(tmpFile, quickLZstate);
}

void SerializableFlannBasedMatcher::loadIndex(cv::Mat * data,
		const string & tmpDir) {
	string tmpFile = tmpDir + this->filename + ".if";
	cv::flann::IndexParams* params = new cv::flann::SavedIndexParams(
			tmpFile.c_str());
	//FIXME can cause memory leaks
	flannIndex = makePtr<cv::flann::Index>(*data, *params);
	delete params; //FIXME can cause problems
}

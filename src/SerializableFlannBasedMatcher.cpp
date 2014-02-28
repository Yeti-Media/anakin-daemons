#include "SerializableFlannBasedMatcher.hpp"
#include <iostream>               // for std::cout
#include <stdio.h>
#include <fstream>      // std::ifstream


using namespace Anakin;
using namespace cv;


SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
                                const Ptr<flann::IndexParams>& indexParams,
                                const Ptr<flann::SearchParams>& searchParams) : FlannBasedMatcher(indexParams, searchParams){
    this->loadedFromFile = false;
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(std::string filename) {
    this->filename = filename;
    this->load();
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(std::string xmlData, std::string filename) {
    this->filename = filename;
    this->load(xmlData);
}

void SerializableFlannBasedMatcher::save(std::string filename, std::string * xmlData) {
    this->filename = filename;
    bool saveXMLtoFile = xmlData == NULL;

    std::string fbmData;
    cv::FileStorage fs(fbmData, cv::FileStorage::WRITE | cv::FileStorage::MEMORY);
    write(fs);
    fbmData = fs.releaseAndGetString();

    cv::FileStorage mfs(saveXMLtoFile?(filename+".xml"):"output.xml", saveXMLtoFile?cv::FileStorage::WRITE : (cv::FileStorage::WRITE | cv::FileStorage::MEMORY));

    mfs << "fbmData" << fbmData;
    cv::DescriptorMatcher::DescriptorCollection* mdptr = &mergedDescriptors;
    mdptr++;
    mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((std::vector<int> *)mdptr - 1);
    std::vector<int>* startIdxsPtr = ((std::vector<int> *)mdptr);
    mfs << "mergedDescriptorsstartIdxs" << "[";
    for (uint i = 0; i < startIdxsPtr->size(); i++) {
        mfs << startIdxsPtr->at(i);
    }
    mfs << "]";

    mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((cv::Mat *)mdptr - 1);
    cv::Mat* mergedDescriptorsDescriptors = ((cv::Mat *)mdptr);
    mfs << "mergedDescriptorsDescriptors" << *mergedDescriptorsDescriptors;

    if (saveXMLtoFile) {
        mfs.release();
    } else {
        *xmlData = mfs.releaseAndGetString();
    }

    saveIndex();
}

bool SerializableFlannBasedMatcher::empty() const {
    if (this->loadedFromFile) return false;
    return trainDescCollection.empty();
}

//PRIVATE

void SerializableFlannBasedMatcher::load(std::string xmlData) {
    bool loadXMLfromFile = xmlData.empty();
    cv::FileStorage fs(loadXMLfromFile?(filename+".xml"):(xmlData), loadXMLfromFile?(cv::FileStorage::READ):(cv::FileStorage::READ | cv::FileStorage::MEMORY));

    cv::FileNode root = fs.root();
    std::string sfbmData = root["fbmData"];
    cv::FileStorage fbmDatafs(sfbmData, cv::FileStorage::READ | cv::FileStorage::MEMORY);
    read(fbmDatafs.root());

    cv::DescriptorMatcher::DescriptorCollection* mdptr = &mergedDescriptors;
    mdptr++;
    mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((std::vector<int> *)mdptr - 1);
    std::vector<int>* startIdxsPtr = ((std::vector<int> *)mdptr);

    cv::FileNode mdsidxnd = root["mergedDescriptorsstartIdxs"];
    cv::FileNodeIterator it = mdsidxnd.begin(), it_end = mdsidxnd.end(); // Go through the node
    for (; it != it_end; ++it)
        startIdxsPtr->push_back((int)*it);

    mdptr = (cv::DescriptorMatcher::DescriptorCollection*) ((cv::Mat *)mdptr - 1);
    cv::Mat* mergedDescriptorsDescriptors = ((cv::Mat *)mdptr);

    root["mergedDescriptorsDescriptors"] >> *mergedDescriptorsDescriptors;
    addedDescCount = mergedDescriptorsDescriptors->rows;

    loadIndex(*mergedDescriptorsDescriptors);
    this->loadedFromFile = true;
}

void SerializableFlannBasedMatcher::saveIndex() {
    std::string tmpFile = this->filename+".if";
    flannIndex->save(tmpFile);
//    std::ifstream is (tmpFile.c_str(), std::ifstream::binary);
//    is.seekg (0, is.end);
//    int length = is.tellg();
//    is.seekg (0, is.beg);
//    char * buffer = new char [length];
//    is.read (buffer,length);
//    is.close();
//    remove(tmpFile.c_str());
}

void SerializableFlannBasedMatcher::loadIndex(cv::Mat data) {
    std::string tmpFile = this->filename+".if";
    cv::flann::IndexParams* params = new cv::flann::SavedIndexParams(tmpFile.c_str());
    flannIndex = new cv::flann::Index(data, *params);
}

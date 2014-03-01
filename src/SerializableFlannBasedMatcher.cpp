#include "SerializableFlannBasedMatcher.hpp"
#include <iostream>               // for std::cout
#include <stdio.h>
#include <fstream>      // std::ifstream
#include <sstream>
//#include <time.h>
#include "quicklz.h"
#define QLZ_STREAMING_BUFFER 0


using namespace Anakin;
using namespace cv;


SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(
                                const Ptr<flann::IndexParams>& indexParams,
                                const Ptr<flann::SearchParams>& searchParams) : FlannBasedMatcher(indexParams, searchParams){
    this->loadedFromFile = false;
}

SerializableFlannBasedMatcher::SerializableFlannBasedMatcher(std::string filename, bool removeFileAfterLoad) {
    this->filename = filename;
    std::string* xmlData = new std::string();
    decompress(true, xmlData);
    this->load(*xmlData);
    if (removeFileAfterLoad) {
        std::string indexFile = filename+".if";
        std::string matcherFile = filename+".xml";
        remove(indexFile.c_str());
        remove(matcherFile.c_str());
    }
    delete xmlData;
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
    compress(true);
}

bool SerializableFlannBasedMatcher::empty() const {
    if (this->loadedFromFile) return false;
    return trainDescCollection.empty();
}

void SerializableFlannBasedMatcher::train(std::vector<cv::Mat> descriptors) {
    mergedDescriptors.set( descriptors );
    flannIndex = new flann::Index( mergedDescriptors.getDescriptors(), *indexParams );
}

//PRIVATE

void SerializableFlannBasedMatcher::load(std::string xmlData) {
    bool loadXMLfromFile = xmlData.empty();
    std::string file = loadXMLfromFile?(filename+".xml"):(xmlData);
    cv::FileStorage fs(file, loadXMLfromFile?(cv::FileStorage::READ):(cv::FileStorage::READ | cv::FileStorage::MEMORY));
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
}

void SerializableFlannBasedMatcher::loadIndex(cv::Mat data) {
    std::string tmpFile = this->filename+".if";
    cv::flann::IndexParams* params = new cv::flann::SavedIndexParams(tmpFile.c_str());
    flannIndex = new cv::flann::Index(data, *params);
}

void SerializableFlannBasedMatcher::compress(bool removeOriginal) {
    std::string uncompressedIndexFileName = this->filename+".if";
    std::string uncompressedMatcherFileName = this->filename+".xml";
    std::string compressedIndexFileName = removeOriginal?uncompressedIndexFileName:this->filename+".cif";
    std::string compressedMatcherFileName = removeOriginal?uncompressedMatcherFileName:this->filename+".cxml";
    char * uindexData;
    char * umatcherData;
    int uindexDataSize;
    int umatcherDataSize;
    uindexDataSize = readFile(&uindexData, uncompressedIndexFileName);
    umatcherDataSize = readFile(&umatcherData, uncompressedMatcherFileName);
    if (removeOriginal) {
        remove(uncompressedIndexFileName.c_str());
        remove(uncompressedMatcherFileName.c_str());
    }
    char * compressedIndexData = (char *)malloc(uindexDataSize + 400);
    char * compressedMatcherData = (char *)malloc(umatcherDataSize + 400);
    qlz_state_compress *state_compress_index = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
    qlz_state_compress *state_compress_matcher = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
    int ir;
    int mr;
    //clock_t t_1 = clock();
    ir = qlz_compress(uindexData, compressedIndexData, uindexDataSize, state_compress_index);
    mr = qlz_compress(umatcherData, compressedMatcherData, umatcherDataSize, state_compress_matcher);
    //clock_t t_2 = clock();
    //float tt = ((float)(t_2 - t_1))/CLOCKS_PER_SEC;
    //std::cout << "compressing time: " << tt << std::endl;
    writeFile(compressedIndexData, compressedIndexFileName, ir);
    writeFile(compressedMatcherData, compressedMatcherFileName, mr);
    delete state_compress_index;
    delete state_compress_matcher;
    delete uindexData;
    delete umatcherData;
    delete compressedIndexData;
    delete compressedMatcherData;
}

void SerializableFlannBasedMatcher::decompress(bool useOriginalNames, std::string * xmlData) {
    bool decompressMatcherToFile = xmlData == NULL;
    std::string uncompressedIndexFileName = this->filename+(useOriginalNames?".if":".uif");
    std::string uncompressedMatcherFileName = this->filename+(useOriginalNames?".xml":".uxml");
    std::string compressedIndexFileName = this->filename+(useOriginalNames?".if":".cif");
    std::string compressedMatcherFileName = this->filename+(useOriginalNames?".xml":".cxml");
    char * cindexData;
    char * cmatcherData;
    readFile(&cindexData, compressedIndexFileName);
    readFile(&cmatcherData, compressedMatcherFileName);
    if (useOriginalNames) {
        remove(compressedIndexFileName.c_str());
        remove(compressedMatcherFileName.c_str());
    }
    int ucompressedIndexDataSize = qlz_size_decompressed(cindexData);
    int ucompressedMatcherDataSize = qlz_size_decompressed(cmatcherData);
    char * ucompressedIndexData = (char *)malloc(ucompressedIndexDataSize);
    char * ucompressedMatcherData = (char *)malloc(ucompressedMatcherDataSize);
    qlz_state_decompress *state_decompress_index = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
    qlz_state_decompress *state_decompress_matcher = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
    int ir;
    int mr;
    //clock_t t_1 = clock();
    ir = qlz_decompress(cindexData, ucompressedIndexData, state_decompress_index);
    mr = qlz_decompress(cmatcherData, ucompressedMatcherData, state_decompress_matcher);
    //clock_t t_2 = clock();
    //float tt = ((float)(t_2 - t_1))/CLOCKS_PER_SEC;
    //std::cout << "decompressing time: " << tt << std::endl;
    writeFile(ucompressedIndexData, uncompressedIndexFileName, ir);
    if (decompressMatcherToFile) {
        writeFile(ucompressedMatcherData, uncompressedMatcherFileName, mr);
    } else {
        std::string loadedMatcherDara(ucompressedMatcherData, mr);
        *xmlData = loadedMatcherDara;
    }
    delete state_decompress_index;
    delete state_decompress_matcher;
    delete cindexData;
    delete cmatcherData;
    delete ucompressedIndexData;
    delete ucompressedMatcherData;
}

int SerializableFlannBasedMatcher::readFile(char ** data, std::string filename) {
    std::ifstream is (filename.c_str(), std::ifstream::binary);
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    *data = new char [length];
    is.read (*data,length);
    is.close();
    return length;
}

 void SerializableFlannBasedMatcher::writeFile(char * data, std::string filename, int length) {
    std::ofstream os (filename.c_str(), std::ofstream::out | std::ofstream::binary);
    os.write(data, length);
    os.close();
 }

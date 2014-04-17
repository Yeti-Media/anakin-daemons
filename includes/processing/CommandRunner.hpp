#ifndef COMMANDRUNNER_HPP
#define COMMANDRUNNER_HPP

#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"
#include <string>
#include <iostream>               // for std::cout
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "matching/BasicFlannDetector.hpp"
#include "processing/Flags.hpp"
#include "output/ResultWriter.hpp"
#include "output/DataOutput.hpp"
#include "matching/SerializableFlannBasedMatcher.hpp"
#include "processing/SFBMCache.hpp"
#include "data/ImageInfo.hpp"
#include "matching/FlannMatchingProcessor.hpp"

namespace fs = boost::filesystem;

namespace Anakin
{

/**
* This class will process a request
*
* the process can be defined by the following steps
*
* 1 - validate the request using a Flags object
* 2 - extract flags and values from the request
* 3 - validate the extracted flags and values
* 4 - execute an action based on the extracted flags and values
* 5 - send a response for the request using a DataOutput object
*
*/
class CommandRunner
{
public:
    /**
    * Constructor (does steps 1, 2, 3)
    * flags : used to validate the request
    * out   : used to send responses
    * input : the request to process
    */
    CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache, std::vector<std::string> *input);

    CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache);

    void validateRequest(std::vector<std::string> *input);

    /**
    * run the command runner (does steps 4 and 5)
    */
    int run();
    static const char MATCH = 1;
    static const char ADDIDXS = 2;
    static const char DELIDXS = 3;
    static const char UPDIDXS = 4;
    static const char IDXSTATUS = 5;
protected:
private:
    char action = 0;
    int mma = 8;
    float mr = 1.f / 1.5f;
    int sceneID;
    std::vector<std::string> indexes;
    std::string reqID;
    std::string error;
    bool inputError = false;
    DataOutput* out;
    ResultWriter* rw;
    SFBMCache* cache;
    FlannMatchingProcessor* processor = NULL;
    BasicFlannDetector* detector = NULL;
    Flags* flags;
};
};

#endif // COMMANDRUNNER_HPP

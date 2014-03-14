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

#include <BasicFlannDetector.hpp>
#include <Flags.hpp>
#include <ResultWriter.hpp>
#include <DataOutput.hpp>
#include "SerializableFlannBasedMatcher.hpp"
#include "SFBMCache.hpp"
#include "ImageInfo.hpp"
#include "FlannMatchingProcessor.hpp"

namespace fs = boost::filesystem;

namespace Anakin {

class CommandRunner {
    public:
        CommandRunner(Flags* flags, DataOutput* out, SFBMCache* cache, std::vector<std::string> *input);
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
        std::string sceneID;
        bool useSceneIDasPath = false;
        std::vector<std::string> indexes;
        std::string reqID;
        std::string error;
        bool inputError = false;
        DataOutput* out;
        ResultWriter* rw;
        SFBMCache* cache;
        FlannMatchingProcessor* processor = NULL;
        BasicFlannDetector* detector = NULL;
};
};

#endif // COMMANDRUNNER_HPP

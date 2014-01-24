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

#include <ImageDataInput.hpp>
#include <JustShowImageProcessor.hpp>
#include <Detector.hpp>
#include <PatternLoader.hpp>
#include <BasicImageProcessor.hpp>
#include <BasicFlannDetector.hpp>
#include <SingleImageDataInput.hpp>
#include <HistogramComparator.hpp>
#include <Flags.hpp>
#include <HistMatch.hpp>
#include <ResultWriter.hpp>
#include <Histogram.hpp>
#include <OCRDetector.hpp>
#include <FaceDetector.hpp>
#include <FaceMatch.hpp>
#include <ImageComponentExtractor.hpp>
#include <SteinNormalizer.hpp>
#include <SerializedPatternDataInput.hpp>
#include <HistogramsIO.hpp>
#include <DataOutput.hpp>

namespace fs = boost::filesystem;

namespace Anakin {

class CommandRunner {
    public:
        CommandRunner(Flags* flags, DataOutput* out, std::vector<std::string> *input, bool showHelpOnError=true);
        int run();
        void showHelp();
    protected:
    private:
        bool useScenePathAsDir = false;
        int mma = 8;
        float mr = 1.f / 1.5f;
        char mode = 0;
        bool useHistComparison = false;
        bool useLandscapeComparison = false;
        float minHistPercentage = 0;
        float histSafeOffset = 0;
        bool run_ocr_demo = false;
        bool run_ocr_adv_demo = false;
        bool run_ocr_detect = false;
        bool show = false;
        int clearEvery = 0;
        bool face_detection = false;
        std::vector<std::pair<cv::Point*, cv::Point*>>* ocrRois;
        std::string landscapeLabel = "landscape";
        std::string scenesDir = "landscapes/forest";
        std::string patternsDir = "tests/fixtures/images";
        std::vector<std::string>* detailsCC;
        std::string mainCC;
        bool useDetailsDetection=false;
        float scaleFactor = 1.1;
        int minNeighbors = 3;
        cv::Size minSize = Size();
        cv::Size maxSize = Size();
        bool run_contour_demo = false;
        int aproxMode = CV_CHAIN_APPROX_NONE;
        int thresBegin = 0;
        int thresEnd = 256;
        int thresMode = CV_THRESH_BINARY;
        bool useEdgeDetection = false;
        double threshold;
        int apertureSize=3;
        bool L2gradient=false;
        int ratio=3;
        bool useBlur = false;
        bool useEdgeDetectionToFindContours=false;
        int ocrMode = 0;
        std::string lang = "eng";
        std::string datapath = "";
        bool loadFromImages = true;
        bool saveToYML = false;
        std::string patternsYMLOutputDir = "";
        bool showhelp = false;
        std::string error;
        bool inputError = false;
        DataOutput* out;
        std::string userID;
        bool loadFromDB=false;
};
};

#endif // COMMANDRUNNER_HPP

/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

////////////////////////////////////////////////////////////////////
// File includes:

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>               // for std::cout
#include <execinfo.h>
#include <signal.h>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

                                  //  a namespace alias is preferred practice in real code

#define CONSOLE 1 //1 : run from console, 0 : run from IDE
#define VERBOSE 1
#define USE_HIST_COMP 0
#define USE_LANDSCAPE_COMP 0
#define USE_SCENES_PATH_AS_FOLDER 0
#define RUN_OCR_DEMO 0
#define RUN_OCR_ADV_DEMO 0
#define RUN_OCR_DETECT 0
#define SHOW 0
#define CLEAR_EVERY 0;
#define FACE_DETECTION 0;


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

using namespace Anakin;

namespace fs = boost::filesystem;          // for ease of tutorial presentation;

extern int test();

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

void showHelp() {
    cout << "Anakin help\n\n";
    cout << "for help use -help (duh!)\n";
    cout << "\ocr, lanscape and histogram arguments\n";
    cout << "-s <path to scene file> : scene file refers to the image that will be tested\n";
    cout << "-S <path to scene folder> : will test all images inside a folder\n";
    cout << "-p <path to patterns folder> : patterns refer to images againts a scene will be tested\n";
    cout << "\nocr, landscape and face common arguments\n";
    cout << "-show : this enables UI output\n";
    cout << "   * when doing ocr detection this will show the input image and a green rectangle for each rectangle passed as argument\n";
    cout << "   * when doing landscape detection this will show the histogram constructed from the patterns images with min, max and avg values\n";
    cout << "   * when doing face detection this will show main and details features detected\n";
    cout << "\nhistogram, landscape common arguments\n";
    cout << "-min <value> : this will set the minimum accepted value when testing a scene\n";
    cout << "   * when doing histogram matching this value will refer to the minimum matching pertentage\n";
    cout << "     the scene and pattern histograms must have to be a succesfull match\n";
    cout << "   * when doing landscape matching this value will refer to the minimum percentage the values of the scene histogram matches the landscape histogram\n";
    cout << "       * if the matching use the min max values of the landscape histogram then a scene histogram value match if it's between the corresponding\n";
    cout << "         lanscape min and max value\n";
    cout << "       * if the matching use the avg values of the landscape histogram then a scene histogram value match if it's equal to the corresponding\n";
    cout << "         landscape histogram avg value\n";
    cout << "\ntemplate specific arguments\n";
    cout << "-mma <value> : this will set the minimum matches a pair of scene/pattern must have to be a succesfull match\n";
    cout << "-mr <min ratio> : filter matches where the distance ratio between nearest matches is greater than min ratio\n";
    cout << "\nhistogram specific arguments\n";
    cout << "-corr : will use correlation method when comparing histograms\n";
    cout << "-inter : will use intersection method when comparing histograms\n";
    cout << "\nlandscape specific arguments\n";
    cout << "-minMax(default value) : this will use the min values and max values of the histograms of the landscapes\n";
    cout << "-avg : this will use the average value of the landscapes histograms\n";
    cout << "-safeOffset <value> : when comparing histogram value x this will also compare with x+value and x-value\n";
    cout << "   *when using min max values of the landscapes histogram, an x value from the scene histogram will match if x is in [min-value..max+value]\n";
    cout << "   *when using average values of the landscapes histogram, an x value from the scene histogram will match if x is in [avg-value..avg+value]\n";
    cout << "-label <value> : this will set the landscape label to value\n";
    cout << "-color : will use color to make landscape and scene histograms\n";
    cout << "-gray : will use gray to make landscape and scene histograms\n";
    cout << "-hsv : will use hue and saturation to make landscape and scene histograms\n";
    cout << "   NOTE: if there's neither -color, -gray or -hsv specified then the matching will be made using all three and taking the max value for each\n";
    cout << "\nocr specific arguments\n";
    cout << "-rois <p1x p1y p2x p2y>+ : will define rectangles in which ocr recognition will be executed\n";
    cout << "-clearEvery <times> : will clear tesseract memory every times recognitions\n";
    cout << "\n\ntemplate matching\n";
    cout << "./anakin2 (-s <value>|-S <value>) -p <value> [template matching arguments]\n";
    cout << "\n\nhistogram matching\n";
    cout << "./anakin2 (-s <value>|-S <value>) -p <value> (-h | -hColor | -hHSV | -hGray) [histogram matching arguments]\n";
    cout << "-hColor : will use color histograms for matching\n";
    cout << "-hGray : will use gray histograms for matching\n";
    cout << "-hHSV : will use hue and saturation histograms for matching\n";
    cout << "-h : will use all of the above histograms and use the maximum value obtained for matching\n";
    cout << "\n\nlandscape matching\n";
    cout << "./anakin2 -landscape (-s <value>|-S <value>) -p <value> [landscape matching arguments]\n";
    cout << "\n\nocr detection\n";
    cout << "./anakin2 -ocr <path to image> [ocr arguments]\n";
    cout << "\n\nocr basic demo\n";
    cout << "./anakin2 -ocrDemo\n";
    cout << "\n\nocr advanced demo\n";
    cout << "./anakin2 -ocrAdvDemo\n";
    cout << "\n\nface detection\n";
    cout << "./anakin2 -face <path to image> -mainCC <path to xml> [-detailsCC <path to xml>+] [-scaleFactor <value> -minNeighbors <value> -minSize <value> -maxSize <value>]\n";
    cout << "-face <path to image> : will use face detection on the specified image\n";
    cout << "-mainCC <path to xml> : the classifier file used to detect main features\n";
    cout << "-detailsCC <path to xml>+ : classifier fiels used to detect details inside detected main features\n";
    cout << "-scaleFactor <value> : specify how much the image size is reduced at each image scale.(default 1.1)\n";
    cout << "-minNeighbors <value> :  specify how many neighbors each candidate rectangle should have to retain it.(default 3)\n";
    cout << "-minSize <width> <height> : minimum possible object size. Objects smaller than that are ignored.(default none)\n";
    cout << "-maxSize <width> <height> : Maximum possible object size. Objects larger than that are ignored.(default none)\n";
    cout << "\nNOTE: the order of the arguments doesn't matter (it only matters the order -flag [<values>])\n";
}

int main(int argc, const char * argv[]) {
    signal(SIGSEGV, handler);
    bool useScenePathAsDir = USE_SCENES_PATH_AS_FOLDER;
    bool sceneArgFound = false;
    int mma = 8;
    float mr = 1.f / 1.5f;
    char mode = 0;
    bool useHistComparison = USE_HIST_COMP;
    bool useLandscapeComparison = USE_LANDSCAPE_COMP;
    float minHistPercentage = 0;
    float histSafeOffset = 0;
    bool run_ocr_demo = RUN_OCR_DEMO;
    bool run_ocr_adv_demo = RUN_OCR_ADV_DEMO;
    bool run_ocr_detect = RUN_OCR_DETECT;
    bool show = SHOW;
    int clearEvery = CLEAR_EVERY;
    bool face_detection = FACE_DETECTION;
    std::vector<std::pair<cv::Point*, cv::Point*>>* ocrRois = new std::vector<std::pair<cv::Point*, cv::Point*>>(0);
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



    const char * argv_[] = {
         "./anakin2", "-face", "people.jpg",
         "-mainCC",
         "haarcascades/haarcascade_profileface.xml",
         "-show"
    };
    int argc_ = 6;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }

    if (CONSOLE) {
        Flags* flags = new Flags(input);
        flags->setMinCount(1);
        flags->setOverridingFlag("help");
        flags->setOverridingFlag("ocrDemo");
        flags->setOverridingFlag("ocrAdvDemo");

        //keypoints detection
        flags->setOptionalFlag("p"); //patterns folder path
        flags->setOptionalFlag("s"); //scene path
        flags->setOptionalFlag("S"); //scene folder path
        flags->setOptionalFlag("mr");
        flags->setOptionalFlag("mma");
        vector<string>* pLooseDeps = new vector<string>(0);
        pLooseDeps->push_back("s");
        pLooseDeps->push_back("S");
        flags->setLooseDependencies("p", pLooseDeps);
        flags->setDependence("s", "p");
        flags->setDependence("S", "p");
        flags->setDependence("mr", "p");
        flags->setDependence("mma", "p");
        flags->setIncompatibility("s", "S");

        //histogram flags
        flags->setNoValuesFlag("landscape");
        flags->setNoValuesFlag("h");
        flags->setNoValuesFlag("hColor");
        flags->setNoValuesFlag("hGray");
        flags->setNoValuesFlag("hHSV");
        flags->setNoValuesFlag("corr");
        flags->setNoValuesFlag("inter");
        flags->setOptionalFlag("min");
        flags->setOptionalFlag("safeOffset"); //ONLY FOR LANDSCAPE FOR NOW
        flags->setIncompatibility("h", "hColor");
        flags->setIncompatibility("h", "hGray");
        flags->setIncompatibility("h", "hHSV");
        flags->setIncompatibility("hColor", "hGray");
        flags->setIncompatibility("hColor", "hHSV");
        flags->setIncompatibility("hGray", "hHSV");
        flags->setIncompatibility("h", "mr");
        flags->setIncompatibility("h", "mma");
        flags->setIncompatibility("hColor", "mr");
        flags->setIncompatibility("hColor", "mma");
        flags->setIncompatibility("hGray", "mr");
        flags->setIncompatibility("hGray", "mma");
        flags->setIncompatibility("hHSV", "mr");
        flags->setIncompatibility("hHSV", "mma");
        flags->setIncompatibility("corr", "inter");
        flags->setDependence("min", "p");
        flags->setDependence("h", "p");
        flags->setDependence("hColor", "p");
        flags->setDependence("hGray", "p");
        flags->setDependence("hHSV", "p");
        flags->setDependence("corr", "p");
        flags->setDependence("inter", "p");
        vector<string>* minLooseDeps = new vector<string>(0);
        minLooseDeps->push_back("h");
        minLooseDeps->push_back("hColor");
        minLooseDeps->push_back("hGray");
        minLooseDeps->push_back("hHSV");
        minLooseDeps->push_back("landscape");
        vector<string>* safeOffsetLooseDeps = new vector<string>(0);
        safeOffsetLooseDeps->push_back("h");
        safeOffsetLooseDeps->push_back("hColor");
        safeOffsetLooseDeps->push_back("hGray");
        safeOffsetLooseDeps->push_back("hHSV");
        safeOffsetLooseDeps->push_back("landscape");
        flags->setLooseDependencies("min", minLooseDeps);
        flags->setLooseDependencies("corr", minLooseDeps);
        flags->setLooseDependencies("inter", minLooseDeps);
        flags->setLooseDependencies("safeOffset", safeOffsetLooseDeps);

        //landscape flags
        flags->setDependence("landscape", "p");
        flags->setOptionalFlag("label");
        flags->setDependence("label", "landscape");
        flags->setIncompatibility("landscape", "h");
        flags->setIncompatibility("landscape", "hColor");
        flags->setIncompatibility("landscape", "hGray");
        flags->setIncompatibility("landscape", "hHSV");
        flags->setIncompatibility("landscape", "mr");
        flags->setIncompatibility("landscape", "mma");
        flags->setIncompatibility("landscape", "ocr");
        flags->setNoValuesFlag("color");
        flags->setNoValuesFlag("hsv");
        flags->setNoValuesFlag("gray");
        flags->setDependence("color", "landscape");
        flags->setDependence("hsv", "landscape");
        flags->setDependence("gray", "landscape");
        flags->setNoValuesFlag("minMax");
        flags->setNoValuesFlag("avg");
        flags->setIncompatibility("minMax", "avg");
        flags->setDependence("minMax", "landscape");
        flags->setDependence("avg", "landscape");

        //OCR flags
        flags->setOptionalFlag("ocr");
        flags->setIncompatibility("ocr", "p");
        flags->setOptionalFlag("rois");
        flags->setDependence("rois", "ocr");
        flags->setIncompatibility("ocr", "h");
        flags->setIncompatibility("ocr", "hColor");
        flags->setIncompatibility("ocr", "hGray");
        flags->setIncompatibility("ocr", "hHSV");
        flags->setIncompatibility("ocr", "mr");
        flags->setIncompatibility("ocr", "mma");
        flags->setIncompatibility("ocr", "landscape");
        flags->setOptionalFlag("clearEvery");
        flags->setDependence("clearEvery", "ocr");
        flags->setIncompatibility("clearEvery", "landscape");

        flags->setNoValuesFlag("show");
        vector<string>* showLooseDeps = new vector<string>(0);
        safeOffsetLooseDeps->push_back("landscape");
        safeOffsetLooseDeps->push_back("ocr");
        safeOffsetLooseDeps->push_back("face");
        flags->setLooseDependencies("show", showLooseDeps);
        flags->setIncompatibility("show", "ocrDemo");
        flags->setIncompatibility("show", "ocrAdvDemo");

        //FACE DETECTION
        flags->setOptionalFlag("face");
        flags->setOptionalFlag("mainCC");
        flags->setOptionalFlag("detailsCC");
        flags->setIncompatibility("face", "p");
        flags->setIncompatibility("face", "ocr");
        flags->setIncompatibility("face", "ocrDemo");
        flags->setIncompatibility("face", "ocrAdvDemo");
        flags->setIncompatibility("face", "landscape");
        flags->setIncompatibility("face", "h");
        flags->setIncompatibility("face", "hColor");
        flags->setIncompatibility("face", "hGray");
        flags->setIncompatibility("face", "hHSV");
        flags->setDependence("detailsCC", "face");
        flags->setDependence("mainCC", "face");
        flags->setDependence("face", "mainCC");
        flags->setOptionalFlag("scaleFactor");
        flags->setOptionalFlag("minNeighbors");
        flags->setOptionalFlag("minSize");
        flags->setOptionalFlag("maxSize");
        flags->setDependence("scaleFactor", "face");
        flags->setDependence("minNeighbors", "face");
        flags->setDependence("minSize", "face");
        flags->setDependence("maxSize", "face");

        flags->setVerbose(VERBOSE);

        if (flags->validateInput()) {
            if (flags->flagFound("help")) {
                showHelp();
                return 0;
            }
            if (flags->flagFound("ocrDemo")) {
                OCRDetector* ocrDetector = new OCRDetector("eng", "ocrTest.png");
                ocrDetector->basic_demo();
                return 0;
            }
            if (flags->flagFound("ocrAdvDemo")) {
                OCRDetector* ocrDetector = new OCRDetector("eng", "ocrTest.png");
                ocrDetector->demo();
                return 0;
            }
            vector<string>* values;
            if (flags->flagFound("p")) {
                values = flags->getFlagValues("p");
                if (values->size() == 1) {
                    patternsDir = values->at(0);
                } else {
                    cout << "param p need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("s")) {
                useScenePathAsDir = false;
                sceneArgFound = true;
                values = flags->getFlagValues("s");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    cout << "param s need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("S")) {
                useScenePathAsDir = true;
                sceneArgFound = true;
                values = flags->getFlagValues("S");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    cout << "param S need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("mr")) {
                values = flags->getFlagValues("mr");
                if (values->size() == 1) {
                    mr = stof(values->at(0));
                } else {
                    cout << "param mr need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("mma")) {
                values = flags->getFlagValues("mma");
                if (values->size() == 1) {
                    mma = stoi(values->at(0));
                } else {
                    cout << "param mma need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("ocr")) {
                run_ocr_detect = true;
                values = flags->getFlagValues("ocr");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    cout << "param ocr need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("h")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::COLOR | HistogramComparator::GRAY | HistogramComparator::HSV;
            }
            if (flags->flagFound("hColor")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::COLOR;
            }
            if (flags->flagFound("hGray")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::GRAY;
            }
            if (flags->flagFound("hHSV")) {
                useHistComparison = true;
                mode = mode | HistogramComparator::HSV;
            }
            if (flags->flagFound("color")) {
                mode = mode | HistogramComparator::COLOR;
            } else if (flags->flagFound("gray")) {
                mode = mode | HistogramComparator::GRAY;
            } else if (flags->flagFound("hsv")) {
                mode = mode | HistogramComparator::HSV;
            } else {
                mode = mode | HistogramComparator::COLOR;
                //mode = mode | HistogramComparator::GRAY;
                //mode = mode | HistogramComparator::HSV;
            }
            if (flags->flagFound("min")) {
                values = flags->getFlagValues("min");
                if (values->size() == 1) {
                    minHistPercentage = stof(values->at(0));
                } else {
                    cout << "param min need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("safeOffset")) {
                values = flags->getFlagValues("safeOffset");
                if (values->size() == 1) {
                    histSafeOffset = stof(values->at(0));
                } else {
                    cout << "param safeOffset need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("rois")) {
                values = flags->getFlagValues("rois");
                if (values->size() > 0 && values->size()%4 == 0) {
                    for (int v = 0; v < values->size(); v += 4) {
                        int p1x = stof(values->at(v));
                        int p1y = stof(values->at(v+1));
                        int p2x = stof(values->at(v+2));
                        int p2y = stof(values->at(v+3));
                        cv::Point* p1 = new cv::Point(p1x, p1y);
                        cv::Point* p2 = new cv::Point(p2x, p2y);
                        std::pair<cv::Point*,cv::Point*>  roi(p1,p2);
                        ocrRois->push_back(roi);
                    }
                    histSafeOffset = stof(values->at(0));
                } else {
                    cout << "param safeOffset need an number of values multiple of four(>0)\n";
                    showHelp();
                    return -1;
                }
            }
            bool comparisonModeFound = false;
            if (flags->flagFound("corr")) {
                mode = mode | HistogramComparator::CORRELATION;
                comparisonModeFound = true;
            }
            if (flags->flagFound("inter")) {
                mode = mode | HistogramComparator::INTERSECTION;
                comparisonModeFound = true;
            }
            if (!comparisonModeFound) {
                mode = mode | HistogramComparator::CORRELATION;
            }
            if (flags->flagFound("landscape")) {
                useLandscapeComparison = true;
            }
            if (flags->flagFound("minMax")) {
                mode = mode | HistogramComparator::MINMAX;
            } else if (flags->flagFound("avg")) {
                mode = mode | HistogramComparator::AVG;
            } else {
                mode = mode | HistogramComparator::MINMAX;
            }
            if (flags->flagFound("label")) {
                values = flags->getFlagValues("label");
                if (values->size() == 1) {
                    landscapeLabel = values->at(0);
                } else {
                    cout << "param label need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("show")) {
                show = true;
            }
            if (flags->flagFound("clearEvery")) {
                values = flags->getFlagValues("clearEvery");
                if (values->size() == 1) {
                    clearEvery = stoi(values->at(0));
                } else {
                    cout << "param clearEvery need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("face")) {
                face_detection = true;
                useScenePathAsDir = false;
                values = flags->getFlagValues("face");
                if (values->size() == 1) {
                    scenesDir = values->at(0);
                } else {
                    cout << "param face need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("mainCC")) {
                values = flags->getFlagValues("mainCC");
                if (values->size() == 1) {
                    mainCC = values->at(0);
                } else {
                    cout << "param mainCC need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("detailsCC")) {
                useDetailsDetection = true;
                values = flags->getFlagValues("detailsCC");
                detailsCC = values;
            }
            if (flags->flagFound("scaleFactor")) {
                values = flags->getFlagValues("scaleFactor");
                if (values->size() == 1) {
                    scaleFactor = stof(values->at(0));
                } else {
                    cout << "param scaleFactor need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("minNeighbors")) {
                values = flags->getFlagValues("minNeighbors");
                if (values->size() == 1) {
                    minNeighbors = stoi(values->at(0));
                } else {
                    cout << "param minNeighbors need only one value\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("minSize")) {
                values = flags->getFlagValues("minSize");
                if (values->size() == 2) {
                    minSize.width = stoi(values->at(0));
                    minSize.height = stoi(values->at(1));
                } else {
                    cout << "param minSize need two values\n";
                    showHelp();
                    return -1;
                }
            }
            if (flags->flagFound("maxSize")) {
                values = flags->getFlagValues("maxSize");
                if (values->size() == 2) {
                    maxSize.width = stoi(values->at(0));
                    maxSize.height = stoi(values->at(1));
                } else {
                    cout << "param maxSize need two values\n";
                    showHelp();
                    return -1;
                }
            }


//            if (!sceneArgFound) {
//                cout << "missing parameter, need s or S\n";
//                showHelp();
//                return -1;
//            }
        } else {
            cout << "input error!\n";
            showHelp();
            return -1;
        }
    }


    cv::Ptr<cv::FeatureDetector>     fdetector  = new cv::SurfFeatureDetector(400);
    cv::Ptr<cv::DescriptorExtractor> dextractor = new cv::SurfDescriptorExtractor();
    cv::Ptr<cv::DescriptorMatcher>   matcher   = new cv::FlannBasedMatcher();

    std::vector<RichImg> patterns;
    DataInput* patternsDataInput;
    PatternLoader* patternsLoader;
    if (!run_ocr_detect && !face_detection) {
        patternsDataInput = new ImageDataInput(patternsDir);
        patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
        patternsLoader->load();
    }
    DataInput* scenesDataInput;
    if (!useScenePathAsDir) {
        scenesDataInput = new SingleImageDataInput(scenesDir);
    } else {
        scenesDataInput = new ImageDataInput(scenesDir);
    }
    if (useHistComparison) {
        HistogramComparator* hComparator = new HistogramComparator(scenesDataInput, patterns);
        vector<HistMatch*>* results = hComparator->compareHistograms(minHistPercentage, mode);
        wcout << outputResult(results) << "\n";
    } else if (useLandscapeComparison) {
        HistogramComparator* hComparator = new HistogramComparator(scenesDataInput, patterns);
        Histogram* landscape = hComparator->train_minMax(mode, landscapeLabel, show);
        vector<HistMatch*>* results = hComparator->compareHistogramsMinMax(landscape, minHistPercentage, mode, histSafeOffset);
        wcout << outputResult(results) << "\n";
    } else if (run_ocr_detect) {
        OCRDetector* ocrDetector = new OCRDetector("eng", scenesDir);
        vector<string>* results = ocrDetector->detect(ocrRois, show, clearEvery);
        wcout << outputResult(results) << "\n";
    } else if (face_detection) {
        FaceDetector* faceDetector;
        vector<cv::Rect>* mainDetections = new vector<cv::Rect>(0);
        vector<vector<cv::Rect>*>* detailsDetections = new vector<vector<cv::Rect>*>(0);
        Img* faceDetImage;
        if (!scenesDataInput->nextInput(&faceDetImage)) return -1;
        if (useDetailsDetection) {
            faceDetector = new FaceDetector(mainCC, detailsCC);
        } else {
            faceDetector = new FaceDetector(mainCC);
        }
        faceDetector->setScaleFactor(scaleFactor);
        faceDetector->setMinNeighbors(minNeighbors);
        faceDetector->setMinSize(minSize);
        faceDetector->setMaxSize(maxSize);
        vector<FaceMatch*>* matches = faceDetector->detect(faceDetImage->getGrayImg(), mainDetections, detailsDetections);
        wcout << outputResult(matches) << "\n";
        if (show) {
            faceDetector->showDetections(faceDetImage->getImage(), matches);
        }
    } else {
        Detector *detector = new BasicFlannDetector(matcher, patterns, mr, mma);
        detector->init();

        BasicImageProcessor processor(scenesDataInput, detector, fdetector, dextractor);
        processor.start();

        vector<JSONValue*>* results = processor.getResults();
        //dcout << "results size: " << results->size() << "\n";
        for (int r = 0; r < results->size(); r++) {
            JSONValue* current = results->at(r);
            wcout << current->Stringify().c_str() << "\n";
        }
    }

    return 0;
}



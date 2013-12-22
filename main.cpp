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
namespace fs = boost::filesystem;          // for ease of tutorial presentation;
                                  //  a namespace alias is preferred practice in real code

#define CONSOLE 1 //1 : run from console, 0 : run from IDE
#define VERBOSE 1

#include <ImageDataInput.hpp>
#include <CameraDataInput.hpp>
#include <JustShowImageProcessor.hpp>
#include <JustShowCameraProcessor.hpp>
#include <Detector.hpp>
#include <PatternLoader.hpp>
#include <BasicCameraProcessor.hpp>
#include <BasicImageProcessor.hpp>
#include <BasicFlannDetector.hpp>
#include <SingleImageDataInput.hpp>
#include <HistogramComparator.hpp>
#include <Flags.hpp>
#include <HistMatch.hpp>
#include <ResultWriter.hpp>

using namespace Anakin;

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
    cout << "Anakin - help\n";
    cout << "usage: ./anakin2 -s <scene file path> -p <patterns folder path> [aditional options]\n";
    cout << "usage: ./anakin2 -S <scenes folder path> -p <patterns folder path> [aditional options]\n";
    cout << "additional options: \n";
    cout << "-mr <value> : sets BasicFlanDetector minRatio value\n";
    cout << "-mma <value> : sets BasicFlanDetector min_matches_allowed value\n";
    cout << "-h [[-min <value>] [-corr|-inter]] : will use histogram comparison (color, gray and hsv) and return matching percentage, if a min value is provided it will only return a percentage for matches above the minimum\n";
    cout << "-hColor [[-min <value>] [-corr|-inter]] : will use color histogram comparison and return matching percentage, if a min value is provided it will only return a percentage for matches above the minimum\n";
    cout << "-hGray [[-min <value>] [-corr|-inter]] : will use gray histogram comparison and return matching percentage, if a min value is provided it will only return a percentage for matches above the minimum\n";
    cout << "-hHSV [[-min <value>] [-corr|-inter]] : will use hsv histogram comparison and return matching percentage, if a min value is provided it will only return a percentage for matches above the minimum\n";
    cout << "-corr refers to the correlation method used to compare histograms\n";
    cout << "-inter refers to the intersection method used to compare histograms\n";
    cout << "usage: ./anakin2 -help\n";
}

int main(int argc, const char * argv[]) {
    signal(SIGSEGV, handler);
    bool useScenePathAsDir = false;
    bool sceneArgFound = false;
    int mma = 8;
    float mr = 1.f / 1.5f;
    char mode = 0;
    bool useHistComparison = false;
    float minHistPercentage = 0;
    std::string scenesDir = "tests/fixtures/images";
    std::string patternsDir = "tests/fixtures/scenes";

    const char * argv_[] = {"anakin", "-S", "tests/fixtures/scenes", "-p", "tests/fixtures/images", "-h", "-min", "10.0"};
    int argc_ = 8;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }

    if (CONSOLE) {
        Flags* flags = new Flags(input);
        flags->setRequiredFlag("p"); //patterns folder path
        flags->setOptionalFlag("s"); //scene path
        flags->setOptionalFlag("S"); //scene folder path
        flags->setOptionalFlag("mr");
        flags->setOptionalFlag("mma");
        //histogram flags
        flags->setNoValuesFlag("h");
        flags->setNoValuesFlag("hColor");
        flags->setNoValuesFlag("hGray");
        flags->setNoValuesFlag("hHSV");
        flags->setNoValuesFlag("corr");
        flags->setNoValuesFlag("inter");
        flags->setOptionalFlag("min");
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
        flags->setLooseDependencies("min", minLooseDeps);
        flags->setLooseDependencies("corr", minLooseDeps);
        flags->setLooseDependencies("inter", minLooseDeps);


        flags->setDependence("s", "p");
        flags->setDependence("S", "p");
        flags->setDependence("mr", "p");
        flags->setDependence("mma", "p");
        flags->setIncompatibility("s", "S");
        flags->setOverridingFlag("help");
        flags->setVerbose(VERBOSE);
        if (flags->validateInput()) {
            if (flags->flagFound("help")) {
                showHelp();
                return 0;
            }
            vector<string>* values;
            values = flags->getFlagValues("p");
            if (values->size() == 1) {
                patternsDir = values->at(0);
            } else {
                cout << "param p need only one value\n";
                showHelp();
                return -1;
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
            if (!sceneArgFound) {
                cout << "missing parameter, need s or S\n";
                showHelp();
                return -1;
            }
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
    DataInput* patternsDataInput = new ImageDataInput(patternsDir);
    PatternLoader* patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
    patternsLoader->load();
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



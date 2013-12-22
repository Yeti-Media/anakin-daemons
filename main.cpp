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
    cout << "usage: ./anakin2 -h\n";
}

int main(int argc, const char * argv[]) {
    signal(SIGSEGV, handler);
    bool useScenePathAsDir = false;
    bool sceneArgFound = false;
    int mma = 8;
    float mr = 1.f / 1.5f;
    std::string scenesDir = "tests/fixtures/images";
    std::string patternsDir = "tests/fixtures/scenes";

    const char * argv_[] = {"anakin", "-s", "tests/fixtures/several.jpg", "-p", "tests/fixtures/images/"};
    int argc_ = 5;
    vector<string> *input = new vector<string>(0);
    for (int i = 1; i < argc; i++) {
        input->push_back(argv[i]);
    }

    if (CONSOLE) {
//        if (argc < 3) {
//            std::cout << "wrong number of parameters, need scene path and patterns directory path\n";
//            return -1;
//        }
//        scenesDir = argv[1];
//        patternsDir = argv[2];
        Flags* flags = new Flags(input);
        flags->setRequiredFlag("p"); //patterns folder path
        flags->setOptionalFlag("s"); //scene path
        flags->setOptionalFlag("S"); //scene folder path
        flags->setOptionalFlag("mr");
        flags->setOptionalFlag("mma");
        flags->setDependence("s", "p");
        flags->setDependence("S", "p");
        flags->setDependence("mr", "p");
        flags->setDependence("mma", "p");
        flags->setIncompatibility("s", "S");
        flags->setOverridingFlag("h");
        flags->setVerbose(VERBOSE);
        if (flags->validateInput()) {
            if (flags->flagFound("h")) {
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

    if (!CONSOLE) {
        cv::Mat scene = cv::imread("cocacola.jpg");
//        cv::Mat pattern = cv::imread("scene.jpg");
        Img* sceneImg = new Img(scene, "scene");
//        Img* patternImg = new Img(pattern, "cocacola");
        HistogramComparator hcomp(sceneImg);
//        std::cout << "histogram comparison(color): " << hcomp.compareUsingColor(patternImg, 2) << "%\n";
//        std::cout << "histogram comparison(grayscale): " << hcomp.compareUsingGray(patternImg, 2) << "%\n";
//        std::cout << "histogram comparison(hsv): " << hcomp.compareUsingHSV(patternImg, 2) << "%\n";
        DataInput* trainingSet = new ImageDataInput("landscapes/forest");
        hcomp.train(trainingSet);
        return 0;
    }


    cv::Ptr<cv::FeatureDetector>     fdetector  = new cv::SurfFeatureDetector(400);
    cv::Ptr<cv::DescriptorExtractor> dextractor = new cv::SurfDescriptorExtractor();
    cv::Ptr<cv::DescriptorMatcher>   matcher   = new cv::FlannBasedMatcher();

    std::vector<RichImg> patterns;
    DataInput* patternsDataInput = new ImageDataInput(patternsDir);
    PatternLoader* patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
    patternsLoader->load();
    Detector *detector = new BasicFlannDetector(matcher, patterns, mr, mma);
    detector->init();
    DataInput* scenesDataInput;
    if (!useScenePathAsDir) {
        scenesDataInput = new SingleImageDataInput(scenesDir);
    } else {
        scenesDataInput = new ImageDataInput(scenesDir);
    }
    BasicImageProcessor processor(scenesDataInput, detector, fdetector, dextractor);
    processor.start();

    vector<JSONValue*>* results = processor.getResults();
    //dcout << "results size: " << results->size() << "\n";
    for (int r = 0; r < results->size(); r++) {
        JSONValue* current = results->at(r);
        wcout << current->Stringify().c_str() << "\n";
    }

    return 0;
}



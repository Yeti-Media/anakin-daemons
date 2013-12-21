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

int main(int argc, const char * argv[]) {
    signal(SIGSEGV, handler);
    std::string scenesDir = "tests/fixtures/images";
    std::string patternsDir = "tests/fixtures/scenes";

    if (CONSOLE) {
        if (argc < 3) {
            std::cout << "wrong number of parameters, need scene path and patterns directory path\n";
            return -1;
        }
        scenesDir = argv[1];
        patternsDir = argv[2];
    }

//    if (true) {
//        cv::Mat scene = cv::imread("scene.jpg");
//        cv::Mat pattern = cv::imread("scene.jpg");
//        Img* sceneImg = new Img(scene, "scene");
//        Img* patternImg = new Img(pattern, "cocacola");
//        HistogramComparator hcomp(sceneImg);
//        std::cout << hcomp.compareUsingColor(patternImg, 2) << "%\n";
//        return 0;
//    }

//    std::cout << "Scenes directory : " << scenesDir << "\n";
//    std::cout << "Patterns directory : " << patternsDir << "\n";

    cv::Ptr<cv::FeatureDetector>     fdetector  = new cv::SurfFeatureDetector(400);
    cv::Ptr<cv::DescriptorExtractor> dextractor = new cv::SurfDescriptorExtractor();
    cv::Ptr<cv::DescriptorMatcher>   matcher   = new cv::FlannBasedMatcher();

    std::vector<RichImg> patterns;
    DataInput* patternsDataInput = new ImageDataInput(patternsDir);
    PatternLoader* patternsLoader = new PatternLoader(patternsDataInput, patterns, fdetector, dextractor);
    patternsLoader->load();
    Detector *detector = new BasicFlannDetector(matcher, patterns);
    detector->init();
    DataInput* scenesDataInput;
    if (CONSOLE) {
        scenesDataInput = new SingleImageDataInput(scenesDir);
    } else {
        scenesDataInput = new ImageDataInput(scenesDir);
    }
    BasicImageProcessor processor(scenesDataInput, detector, fdetector, dextractor);
    processor.start();

    return 0;
}



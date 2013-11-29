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
#include "ARDrawingContext.hpp"
#include "ARPipeline.hpp"
#include "DebugHelpers.hpp"

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include "boost/filesystem.hpp"   // includes all needed Boost.Filesystem declarations
#include <iostream>               // for std::cout
#include <execinfo.h>
#include <signal.h>
namespace fs = boost::filesystem;          // for ease of tutorial presentation;
                                  //  a namespace alias is preferred practice in real code

#define DEFAULT_INPUT 0 //0 : cam | 1 : image
#define DEFAULT_IMG "scene.jpg"

/**
 * Processes a recorded video or live view from web-camera and allows you to adjust homography refinement and
 * reprojection threshold in runtime.
 */
bool process(std::vector<Pattern>& patterns, cv::Mat scene, PatternDetector pd, int framesToSkip);

void processCamera(std::vector<cv::Mat>& patternImages, cv::VideoCapture cap, PatternDetector pd);

void processImage(std::vector<cv::Mat>& patternImages, std::string imagePath, PatternDetector pd);

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images);

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
    std::vector<cv::Mat> patterns;
    boost::filesystem::path cwd = boost::filesystem::current_path();
    std::cout << "Working directory : " << cwd.string() << "\n";
    std::string dir_path("/tests/fixtures/images");//argv[1];
    cwd /= dir_path;
    std::cout << "Images directory : " << cwd.string() << "\n";

    bool useCam = false;
    std::string imageToLoad = DEFAULT_IMG;
    if (argc < 2) {
        useCam = DEFAULT_INPUT == 0 ? true : false;
    } else if (argc == 2) {
        //input is 0 or != 0 to use or not camera
        useCam = atoi(argv[1]) == 0 ? true : false;
    } else if (argc == 3) {
        //input is 0 or != 0 to use or not camera
        //image to load
        useCam = atoi(argv[1]) == 0 ? true : false;
        imageToLoad = argv[2];
    }

    getImagesFromDirectory(cwd.string(), patterns);
    PatternDetector pd;
    cv::namedWindow("Cam", CV_WINDOW_AUTOSIZE);

    if (useCam) {
        cv::VideoCapture cap;
        if (cap.open(-1)) {
            processCamera(patterns, cap, pd);
        }
    } else {
        processImage(patterns, imageToLoad, pd);
    }

    return 0;
}

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images) {
  int p = 0;
  if(fs::exists( dir_path )) {
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for (fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr ) {


      if (!fs::is_directory(itr->status())) {
        std::cout << "Loading image : " << itr->path().c_str() << "\n";
        cv::Mat img = cv::imread(itr->path().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
//        cv::namedWindow( "Pattern" + p, CV_WINDOW_AUTOSIZE );
//        cv::namedWindow( "Pattern with keypoints" + p, CV_WINDOW_AUTOSIZE );
//        p++;
        if (!img.data) {
            std::cout << "Error loading image : " << itr->path().c_str() << "\n";
            exit(-1);
        }
        images.push_back(img);
      }
     }
     std::cout << images.size() << " images loaded\n";
  }
}

int correctKeyValue(int original) {
    if (original >= -1 && original <= 255) {
        return original;
    } else {
        return original % 256;
    }
}

void processImage(std::vector<cv::Mat>& patternImages, std::string imagePath, PatternDetector pd) {
    std::vector<Pattern> patternImages_asPatterns(patternImages.size());
    std::vector<cv::Mat> patternImages_keypoints(patternImages.size());

    pd.enableHomographyRefinement;
    pd.homographyReprojectionThreshold = 2;

    for (int p = 0; p < patternImages.size(); p++) {
        pd.buildPatternFromImage(patternImages[p], patternImages_asPatterns[p]);
        if (patternImages_asPatterns[p].keypoints.empty()) {
            std::cout << "empty keypoints!!!!\n";
            //TODO: should discard patterns with empty keypoints
        }
    }
    pd.train(patternImages_asPatterns);
    cv::Mat img = cv::imread(imagePath);
    process(patternImages_asPatterns, img, pd, 0);
    cv::waitKey(0);
}

void processCamera(std::vector<cv::Mat>& patternImages, cv::VideoCapture cap, PatternDetector pd) {
    std::vector<Pattern> patternImages_asPatterns(patternImages.size());
    std::vector<cv::Mat> patternImages_keypoints(patternImages.size());

    pd.enableHomographyRefinement;
    pd.homographyReprojectionThreshold = 2;

    for (int p = 0; p < patternImages.size(); p++) {
        pd.buildPatternFromImage(patternImages[p], patternImages_asPatterns[p]);
        if (patternImages_asPatterns[p].keypoints.empty()) {
            std::cout << "empty keypoints!!!!\n";
            //TODO: should discard patterns with empty keypoints
        }
    }

    pd.train(patternImages_asPatterns);

    bool quit = false;
    while(!quit) {
        cv::Mat frame;
        cap >> frame;
        quit = !process(patternImages_asPatterns, frame, pd, 20);
    }
}

bool process(std::vector<Pattern>& patterns, cv::Mat scene, PatternDetector pd, int framesToSkip) {
    static int frames = 1;
    static bool patternsShowed = true;
    bool keypointsFound = false;
    int skipFrames = framesToSkip == 0 ? 1 : framesToSkip;

//    cv::namedWindow("Scenario", CV_WINDOW_AUTOSIZE);
    if (!patternsShowed) {
        std::vector<cv::Mat> patternsKeypoints(patterns.size());
        for (int p = 0; p < patterns.size(); p++) {
            cv::namedWindow("pattern:"+p, CV_WINDOW_AUTOSIZE);
            cv::drawKeypoints(patterns[p].frame, patterns[p].keypoints, patternsKeypoints[p], cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
            cv::imshow("pattern:"+p, patternsKeypoints[p]);
            cv::waitKey(5);
        }
        patternsShowed = true;
    }


    if (frames % skipFrames== 0 && !scene.empty()) {
        std::cout << "Frames: " << frames << " | searching patterns...\n";

        Pattern framePattern;
        cv::Mat sceneClone = scene;
//        cv::Mat sceneCloneClone = sceneClone.clone();
        cv::Mat frameKeypoints;
        pd.buildPatternFromImage(sceneClone, framePattern);
        keypointsFound = !framePattern.keypoints.empty();
        if (framePattern.keypoints.empty()) {
            std::cout << "no keypoints in frame\n";
            //cv::imshow("Scenario", sceneCloneClone);
        } //else {
//            cv::drawKeypoints(sceneCloneClone, framePattern.keypoints, frameKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
//            cv::imshow("Scenario", frameKeypoints);
//            keypointsFound = true;
//        }
//        cv::waitKey(5);
        if (keypointsFound) {
            bool found = pd.findPattern(sceneClone); //<== current crash pos
            if (found) {
                std::cout << "Pattern found!\n";
                std::vector<cv::DMatch> matches = pd.getMatches();
                std::vector<cv::KeyPoint> roiKeypoints(matches.size());
                std::vector<cv::KeyPoint> patternRoi(matches.size());
                std::vector<cv::Point2f> sceneRoi(matches.size());

                cv::Mat warpedPattern;

                int k = 0;
                for (int m = 0; m < matches.size(); m++) {
                    roiKeypoints[k] = framePattern.keypoints[matches[m].queryIdx];
                    patternRoi[k] = pd.patternMatched.keypoints[matches[m].trainIdx];
                    k++;
                }

                cv::drawKeypoints(sceneClone, framePattern.keypoints, scene, cv::Scalar(255, 50, 0), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
                cv::drawKeypoints(sceneClone, roiKeypoints, scene, cv::Scalar(0, 255, 50), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
                cv::Mat H = pd.getRefinedHomography();
                cv::warpPerspective(pd.patternMatched.frame, warpedPattern, H, pd.patternMatched.frame.size(), cv::INTER_CUBIC);//cv::WARP_NORMAL_MAP | cv::INTER_CUBIC);

                cv::namedWindow("Warped Pattern", CV_WINDOW_AUTOSIZE);
                cv::imshow("Warped Pattern", warpedPattern);
                cv::waitKey(5);

                cv::Point2f cenScenePattern(0,0);
                for ( size_t i=0; i<roiKeypoints.size(); i++ ) {
                    cv::KeyPoint currentPoint = roiKeypoints[i];
                    cenScenePattern.x += currentPoint.pt.x;
                    cenScenePattern.y += currentPoint.pt.y;
                }
                cenScenePattern.x /= roiKeypoints.size();
                cenScenePattern.y /= roiKeypoints.size();

                cv::Point2f cenPattern(0,0);
                for ( size_t i=0; i<pd.patternMatched.keypoints.size(); i++ ) {
                    cv::KeyPoint currentPoint = pd.patternMatched.keypoints[i];
                    cenPattern.x += currentPoint.pt.x;
                    cenPattern.y += currentPoint.pt.y;
                }
                cenPattern.x /= pd.patternMatched.keypoints.size();
                cenPattern.y /= pd.patternMatched.keypoints.size();
                cenPattern.x *= -1;
                cenPattern.y *= -1;

                cv::circle( scene, cenScenePattern, std::max(pd.patternMatched.frame.cols, pd.patternMatched.frame.rows)/4, cv::Scalar( 0, 255, 5 ), 2, 1 );

//                cv::Mat canny_output;
//                std::vector<std::vector<cv::Point> > contours;
//                std::vector<cv::Vec4i> hierarchy;
//
//                /// Detect edges using canny
//                cv::Canny( warpedPattern, canny_output, 50, 100, 3 );
//                /// Find contours
//                cv::findContours( canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cenPattern );
//
//
////                /// Draw contours
//                for( int i = 0; i< contours.size(); i++ ) {
//                   cv::drawContours( scene, contours, i, cv::Scalar(0, 255, 50, 50), 1, 1, hierarchy, 0, cenScenePattern );
//                }

            }
        }
    }
    keypointsFound = false;
    cv::imshow("Cam", scene);
    frames++;
    int keyCode = correctKeyValue(cv::waitKey(90));
    if (keyCode == 27 || keyCode == 'q') return false;
    return true;
}



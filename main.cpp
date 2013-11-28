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
namespace fs = boost::filesystem;          // for ease of tutorial presentation;
                                  //  a namespace alias is preferred practice in real code
/**
 * Processes a recorded video or live view from web-camera and allows you to adjust homography refinement and
 * reprojection threshold in runtime.
 */
bool process(std::vector<Pattern>& patterns, cv::Mat scene, PatternDetector pd, int framesToSkip);

void processCamera(std::vector<cv::Mat>& patternImages, cv::VideoCapture cap, PatternDetector pd);

void processImage(std::vector<cv::Mat>& patternImages, std::string imagePath, PatternDetector pd);

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images);


int main(int argc, const char * argv[]) {
    std::vector<cv::Mat> patterns;
    boost::filesystem::path cwd = boost::filesystem::current_path();
    std::cout << "Working directory : " << cwd.string() << "\n";
    std::string dir_path("/tests/fixtures/images");//argv[1];
    cwd /= dir_path;
    std::cout << "Images directory : " << cwd.string() << "\n";

    getImagesFromDirectory(cwd.string(), patterns);
    PatternDetector pd;
    cv::namedWindow("Cam", CV_WINDOW_AUTOSIZE);
    cv::VideoCapture cap;
    if (cap.open(-1)) {
        processCamera(patterns, cap, pd);
    }
//    processImage(patterns, "scene.jpg", pd);

    return 0;
}

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images)
{
  int p = 0;
  if(fs::exists( dir_path ))
  {
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for (fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
    {


      if (!fs::is_directory(itr->status()))
      {
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
        quit = !process(patternImages_asPatterns, frame, pd, 45);
    }
}

bool process(std::vector<Pattern>& patterns, cv::Mat scene, PatternDetector pd, int framesToSkip) {
    static int frames = 1;
    static bool patternsShowed = true;
    int skipFrames = framesToSkip == 0 ? 1 : framesToSkip;
    cv::Mat sceneClone = scene.clone();
    cv::imshow("Cam", scene);
    cv::waitKey(5);
    std::cout << "MARK1\n";

    cv::namedWindow("Scenario", CV_WINDOW_AUTOSIZE);
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


    if (frames % skipFrames== 0) {
        std::cout << "Frames: " << frames << " | searching patterns...\n";

        Pattern framePattern;
        cv::Mat sceneCloneClone = sceneClone.clone();
        cv::Mat frameKeypoints;
        pd.buildPatternFromImage(sceneClone, framePattern);
        std::cout << "MARK2\n";
        if (framePattern.keypoints.empty()) {
            std::cout << "no keypoints in frame\n";
            cv::imshow("Scenario", sceneCloneClone);
        } else {
            cv::drawKeypoints(sceneCloneClone, framePattern.keypoints, frameKeypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
            cv::imshow("Scenario", frameKeypoints);
        }
        cv::waitKey(5);
        std::cout << "MARK3\n";
        bool found = pd.findPattern(sceneClone); //<== current crash pos
        std::cout << "MARK4\n";
        if (found) {
            std::cout << "Pattern found!\n";
            Pattern sceneAsPattern;
            pd.buildPatternFromImage(sceneClone, sceneAsPattern);
            std::cout << "MARK5\n";
            cv::Mat img_matches;
            cv::drawMatches(sceneClone, sceneAsPattern.keypoints, pd.patternMatched.frame, pd.patternMatched.keypoints,
            pd.getMatches(), img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
            std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
            std::cout << "MARK6\n";
            cv::imshow("Scenario", img_matches);
            cv::waitKey(5);
            std::cout << "MARK7\n";
        }
    }
    frames++;
    int keyCode = correctKeyValue(cv::waitKey(30));
    if (keyCode == 27 || keyCode == 'q') return false;
    return true;
}



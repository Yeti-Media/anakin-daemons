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
bool process(std::vector<cv::Mat>& patternImages, cv::Mat scene);

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images);


int main(int argc, const char * argv[]) {
    std::vector<cv::Mat> patterns;
    boost::filesystem::path cwd = boost::filesystem::current_path();
    std::cout << "Working directory : " << cwd.string() << "\n";
    std::string dir_path("/tests/fixtures/images");//argv[1];
    cwd /= dir_path;
    std::cout << "Images directory : " << cwd.string() << "\n";

    cv::namedWindow( "Scenario", CV_WINDOW_AUTOSIZE );
    cv::Mat scenario = cv::imread("scenario.jpg");

//    cv::imshow("Scenario", scenario);
//    cv::waitKey(0);

    getImagesFromDirectory(cwd.string(), patterns);
    process(patterns, scenario);


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


bool process(std::vector<cv::Mat>& patternImages, cv::Mat scene)
{
    cv::Mat sceneClone = scene.clone();
    std::vector<Pattern> patternImages_asPatterns(patternImages.size());
    std::vector<cv::Mat> pattern_keypoints(patternImages.size());

    // Draw information:
    PatternDetector pd;
    for (int p = 0; p < patternImages.size(); p++) {
//        cv::imshow("Pattern"+p, patternImages[p]);
//        cv::waitKey(5);
        pd.buildPatternFromImage(patternImages[p], patternImages_asPatterns[p]);

        cv::drawKeypoints( patternImages[p], patternImages_asPatterns[p].keypoints, pattern_keypoints[p], cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
        if (patternImages_asPatterns[p].keypoints.empty()) {
            std::cout << "keypoints vacios!!!!\n";
        }
//        cv::imshow("Pattern with keypoints"+p, pattern_keypoints[p]);
//        cv::waitKey(5);

    }
    pd.enableHomographyRefinement;
    pd.homographyReprojectionThreshold = 2;
    pd.train(patternImages_asPatterns);
    bool found = pd.findPattern(sceneClone); //<== current crash pos
    if (found) {
//        cv::Mat H = pd.getRefinedHomography();
//
//
//
//        //-- Get the corners from the image_1 ( the object to be "detected" )
//        std::vector< cv::Point2f > obj_corners(4);
//        obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( pd.patternMatched.frame.cols, 0 );
//        obj_corners[2] = cvPoint( pd.patternMatched.frame.cols, pd.patternMatched.frame.rows ); obj_corners[3] = cvPoint( 0, pd.patternMatched.frame.rows );
//        std::vector< cv::Point2f > scene_corners(4);
//
//        cv::perspectiveTransform( obj_corners, scene_corners, H);
//
//        //-- Draw lines between the corners (the mapped object in the scene - image_2 )
//        cv::line( sceneClone, scene_corners[0] , scene_corners[1], cv::Scalar( 255, 0, 0 ), 2 ); //TOP line
//        cv::line( sceneClone, scene_corners[1] , scene_corners[2], cv::Scalar( 255, 0, 0 ), 2 );
//        cv::line( sceneClone, scene_corners[2] , scene_corners[3], cv::Scalar( 255, 0, 0 ), 2 );
//        cv::line( sceneClone, scene_corners[3] , scene_corners[0] , cv::Scalar( 255, 0, 0 ), 2 );
//        cv::imshow("Scenario", sceneClone);

        Pattern sceneAsPattern;
        pd.buildPatternFromImage(sceneClone, sceneAsPattern);
        cv::Mat img_matches;
        cv::drawMatches(sceneClone, sceneAsPattern.keypoints, pd.patternMatched.frame, pd.patternMatched.keypoints,
        pd.getMatches(), img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
        std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
        cv::imshow("Scenario", img_matches);

        std::cout << "Pattern found!\n";
//        cv::Mat scene_keypoints;
//        cv::drawKeypoints( sceneClone, pd.patternMatched.keypoints, scene_keypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
//        cv::imshow("Scenario", scene_keypoints);
    } else {
        cv::imshow("Scenario", sceneClone);
    }

    cv::waitKey(0);
    return true;
}



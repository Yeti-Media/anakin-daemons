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
void processVideo(std::vector<cv::Mat>& patternImage, CameraCalibration& calibration, cv::VideoCapture capture);

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images);

/**
 * Performs full detection routine on camera frame and draws the scene using drawing context.
 * In addition, this function draw overlay with debug information on top of the AR window.
 * Returns true if processing loop should be stopped; otherwise - false.
 */
bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx);

int main(int argc, const char * argv[])
{
    std::vector<cv::Mat> patterns;
    boost::filesystem::path cwd = boost::filesystem::current_path();
    std::cout << "Working directory : " << cwd.string() << "\n";
    std::string dir_path("/tests/fixtures/images");//argv[1];
    cwd /= dir_path;
    std::cout << "Images directory : " << cwd.string() << "\n";
    // Change this calibration to yours:
    CameraCalibration calibration(526.58037684199849f, 524.65577209994706f, 318.41744018680112f, 202.96659047014398f);

    cv::VideoCapture cap(-1); // open the default camera
    if(!cap.isOpened())  // check if we succeeded
        return -1;
    getImagesFromDirectory(cwd.string(), patterns);
    processVideo(patterns, calibration, cap);
    return 0;
}

void processVideo(std::vector<cv::Mat>& patternImages, CameraCalibration& calibration, cv::VideoCapture capture)
{
    std::cout << "processVideo using " << patternImages.size() << " patterns\n";

    // Grab first frame to get the frame dimensions
    cv::Mat currentFrame;
    capture >> currentFrame;

    // Check the capture succeeded:
    if (currentFrame.empty())
    {
        std::cout << "Cannot open video capture device" << std::endl;
        return;
    }

    cv::Size frameSize(currentFrame.cols, currentFrame.rows);

    ARPipeline pipeline(patternImages, calibration);
    ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

    bool shouldQuit = false;
    do
    {
        capture >> currentFrame;
        if (currentFrame.empty())
        {
            shouldQuit = true;
            continue;
        }

        shouldQuit = processFrame(currentFrame, pipeline, drawingCtx);
    } while (!shouldQuit);
}

void getImagesFromDirectory(std::string dir_path, std::vector<cv::Mat>& images)
{
  if(fs::exists( dir_path ))
  {
    fs::directory_iterator end_itr; // default construction yields past-the-end
    for (fs::directory_iterator itr( dir_path ); itr != end_itr; ++itr )
    {


      if (!fs::is_directory(itr->status()))
      {
        std::cout << "Loading image : " << itr->path().c_str() << "\n";
        cv::Mat img = cv::imread(itr->path().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
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


bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx)
{
    // Clone image used for background (we will draw overlay on it)
    cv::Mat img = cameraFrame.clone();
    // Draw information:
    if (pipeline.m_patternDetector.enableHomographyRefinement)
        cv::putText(img, "Pose refinement: On   ('h' to switch off)", cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
    else
        cv::putText(img, "Pose refinement: Off  ('h' to switch on)",  cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

    cv::putText(img, "RANSAC threshold: " + ToString(pipeline.m_patternDetector.homographyReprojectionThreshold) + "( Use'-'/'+' to adjust)", cv::Point(10, 30), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

    // Set a new camera frame:
    drawingCtx.updateBackground(img);

    // Find a pattern and update it's detection status:
    drawingCtx.isPatternPresent = pipeline.processFrame(cameraFrame);



    // Update a pattern pose:
    drawingCtx.patternPose = pipeline.getPatternLocation();

    // Request redraw of the window:
    drawingCtx.updateWindow();

    // Read the keyboard input:
    int keyCode = correctKeyValue(cv::waitKey(5));
    printf("key pressed: %d\n", keyCode);

    bool shouldQuit = false;
    if (keyCode == '+' || keyCode == '=')
    {
        pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2f;
        pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
    }
    else if (keyCode == '-')
    {
        pipeline.m_patternDetector.homographyReprojectionThreshold -= 0.2f;
        pipeline.m_patternDetector.homographyReprojectionThreshold = std::max(0.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
    }
    else if (keyCode == 'h')
    {
        pipeline.m_patternDetector.enableHomographyRefinement = !pipeline.m_patternDetector.enableHomographyRefinement;
    }
    else if (keyCode == 27 || keyCode == 'q')
    {
        shouldQuit = true;
    }

    return shouldQuit;
}



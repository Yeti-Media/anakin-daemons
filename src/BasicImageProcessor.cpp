#include <opencv2/opencv.hpp>
#include "BasicImageProcessor.hpp"
#include <Match.hpp>
#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;

BasicImageProcessor::BasicImageProcessor(   DataInput* input,
                                            Detector* detector,
                                            Ptr<FeatureDetector>& fdetector,
                                            Ptr<DescriptorExtractor>& dextractor)
: JustShowImageProcessor(input, detector)
{
    this->fdetector = fdetector;
    this->dextractor = dextractor;
}

bool BasicImageProcessor::process(Img& scene) {
    bool skip = false;
    Mat procesedScene;
    if (!skip) {
        RichImg* scenario = new RichImg(&scene, this->fdetector, this->dextractor);
        vector<Match>* matches = this->detector->findPatterns(scenario);
        cout << "======matches: " << matches->size() << " ======\n";
        Mat originalScene = scene.getImage();
        Mat initialScene;
        if (matches->empty()) {
            procesedScene = originalScene;
        } else {
            drawKeypoints(originalScene, scenario->getFreshKeypoints(), procesedScene, Scalar(255, 50, 0), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        }
        for (int m = 0; m < matches->size(); m++) {
            Match match = (*matches)[m];
            Mat patternImg = match.getPattern()->getImage()->getImage();
            Mat sceneImg = match.getScene()->getImage()->getImage();
            vector<cv::KeyPoint> patternKeypoints = match.getPattern()->getKeypoints();
            vector<cv::KeyPoint> sceneKeypoints = match.getMatchedKeypoints();
            vector<DMatch> sceneMatches = *(match.getMatches());

            drawKeypoints(procesedScene, sceneKeypoints, procesedScene, Scalar(0,255,50), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

            vector<Point2f> obj_corners(4);
            obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( patternImg.cols, 0 );
            obj_corners[2] = cvPoint( patternImg.cols, patternImg.rows ); obj_corners[3] = cvPoint( 0, patternImg.rows );
            vector<Point2f> scene_corners(4);

            perspectiveTransform( obj_corners, scene_corners, match.getHomography());

            Point2f offset(0, 0);//Point2f offset(patternImg.cols, 0);

            //-- Draw lines between the corners (the mapped object in the scene - image_2 )
            line( procesedScene, scene_corners[0] + offset, scene_corners[1] + offset, Scalar( 0, 255, 0), 4 );
            line( procesedScene, scene_corners[1] + offset, scene_corners[2] + offset, Scalar( 0, 255, 0), 4 );
            line( procesedScene, scene_corners[2] + offset, scene_corners[3] + offset, Scalar( 0, 255, 0), 4 );
            line( procesedScene, scene_corners[3] + offset, scene_corners[0] + offset, Scalar( 0, 255, 0), 4 );


            string text = match.getPattern()->getImage()->getLabel();
            int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
            double fontScale = 0.8;
            int thickness = 2;

            int baseline=0;
            Size textSize = getTextSize(text, fontFace, fontScale, thickness, &baseline);
            float centerx = match.getCenter().x - (textSize.width/2);
            float centery = match.getCenter().y;
            Point textOrg(centerx, centery);
            putText(procesedScene, text, textOrg, fontFace, fontScale, Scalar(0, 0, 0), thickness, 8);


            wcout << outputResult(match.getCenter(), match.getPattern()->getImage()->getLabel(), match.getMatchedKeypoints()) << "\n";
        }
    } else {
        procesedScene = scene.getImage();
    }
    imshow("Input", procesedScene);
    waitKey();
    return true;
}

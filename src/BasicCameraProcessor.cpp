#include <opencv2/opencv.hpp>
#include "BasicCameraProcessor.hpp"
#include <Match.hpp>

#define FRAMES_TO_SKIP 0

using namespace Anakin;

BasicCameraProcessor::BasicCameraProcessor(
                                            DataInput* input,
                                            Detector* detector,
                                            cv::Ptr<cv::FeatureDetector>& fdetector,
                                            cv::Ptr<cv::DescriptorExtractor>& dextractor)
: JustShowCameraProcessor(input, detector)
{
    this->fdetector = fdetector;
    this->dextractor = dextractor;
}

bool BasicCameraProcessor::process(Anakin::Img& scene) {
    static int frame = 0;
    bool skip = frame < FRAMES_TO_SKIP;
    cv::Mat proccesedScene;
    if (!skip) {
        RichImg* scenario = new RichImg(&scene, this->fdetector, this->dextractor);
        std::vector<Match>* matches = this->detector->findPatterns(scenario);
        std::cout << "======matches: " << matches->size() << " ======\n";
        cv::namedWindow("Processing scene", CV_WINDOW_AUTOSIZE);
        cv::imshow("Processing scene", scene.getImage());
        cv::waitKey();
        cv::destroyWindow("Processing scene");
        cv::Mat originalScene = scene.getImage();
        cv::Mat initialScene;
        if (matches->empty()) {
            proccesedScene = originalScene;
        } else {
            cv::drawKeypoints(originalScene, scenario->getKeypoints(), initialScene, cv::Scalar(255, 50, 0), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        }
        for (int m = 0; m < matches->size(); m++) {
            cv::namedWindow("Pattern found_"+m, CV_WINDOW_AUTOSIZE);

            Match match = (*matches)[m];
            std::vector<cv::DMatch>* dmatches = match.getMatches();
            std::vector<cv::KeyPoint> roiKeypoints(dmatches->size());

            cv::Mat pattern = match.getPattern()->getImage()->getImage();
            cv::Size patternSize = match.getPattern()->getImage()->getSize();
            cv::Mat warpedPattern;


            int k = 0;
            for (int m = 0; m < dmatches->size(); m++) {
                roiKeypoints[k] = match.getPattern()->getKeypoints()[(*dmatches)[m].queryIdx];
                k++;
            }

            cv::drawKeypoints(initialScene, roiKeypoints, proccesedScene, cv::Scalar(0, 255, 50), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

            cv::Mat H = match.getHomography();

            cv::warpPerspective(pattern, warpedPattern, H, patternSize, cv::INTER_CUBIC);//cv::WARP_NORMAL_MAP | cv::INTER_CUBIC);

            cv::imshow("Pattern found_"+m, warpedPattern);
            cv::waitKey(50);

            cv::Point2f cenScenePattern(0,0);
            for ( size_t i=0; i<roiKeypoints.size(); i++ ) {
                cv::KeyPoint currentPoint = roiKeypoints[i];
                cenScenePattern.x += currentPoint.pt.x;
                cenScenePattern.y += currentPoint.pt.y;
            }
            cenScenePattern.x /= roiKeypoints.size();
            cenScenePattern.y /= roiKeypoints.size();

            cv::circle( proccesedScene, cenScenePattern, std::max(patternSize.width, patternSize.height)/4, cv::Scalar( 0, 255, 5 ), 2, 1 );
        }
        frame = 0;
    } else {
        proccesedScene = scene.getImage();
        frame++;
    }
    std::cout << "BasicCameraProcessor#process_4\n";
    cv::imshow("Cam", proccesedScene);
    int keyCode = correctKeyValue(cv::waitKey(50));
    if (keyCode == 27 || keyCode == 'q') return false;
    return true;
}

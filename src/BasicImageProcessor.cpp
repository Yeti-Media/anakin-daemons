#include <opencv2/opencv.hpp>
#include "BasicImageProcessor.hpp"
#include <Match.hpp>

using namespace Anakin;

BasicImageProcessor::BasicImageProcessor(   Anakin::DataInput* input,
                                            Anakin::Detector* detector,
                                            cv::Ptr<cv::FeatureDetector>& fdetector,
                                            cv::Ptr<cv::DescriptorExtractor>& dextractor)
: JustShowImageProcessor(input, detector)
{
    this->fdetector = fdetector;
    this->dextractor = dextractor;
}

bool BasicImageProcessor::process(Anakin::Img& scene) {
    bool skip = false;
    cv::Mat procesedScene;
    if (!skip) {
        RichImg* scenario = new RichImg(&scene, this->fdetector, this->dextractor);
        std::vector<Match>* matches = this->detector->findPatterns(scenario);
        std::cout << "======matches: " << matches->size() << " ======\n";
        cv::Mat originalScene = scene.getImage();
        cv::Mat initialScene;
        if (matches->empty()) {
            procesedScene = originalScene;
        } else {
            cv::drawKeypoints(originalScene, scenario->getKeypoints(), initialScene, cv::Scalar(255, 50, 0), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        }
        for (int m = 0; m < matches->size(); m++) {
            cv::namedWindow("Warped Pattern found_"+m, CV_WINDOW_AUTOSIZE);
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

            cv::drawKeypoints(initialScene, roiKeypoints, procesedScene, cv::Scalar(0, 255, 50), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
            initialScene = procesedScene;
            cv::Mat H = match.getHomography();

            cv::warpPerspective(pattern, warpedPattern, H, patternSize, cv::INTER_CUBIC);//cv::WARP_NORMAL_MAP | cv::INTER_CUBIC);

            cv::imshow("Warped Pattern found_"+m, warpedPattern);
            cv::imshow("Pattern found_"+m, match.getPattern()->getImage()->getImage());
            cv::waitKey();

            cv::Point2f cenScenePattern(0,0);
            for ( size_t i=0; i<roiKeypoints.size(); i++ ) {
                cv::KeyPoint currentPoint = roiKeypoints[i];
                cenScenePattern.x += currentPoint.pt.x;
                cenScenePattern.y += currentPoint.pt.y;
            }
            cenScenePattern.x /= roiKeypoints.size();
            cenScenePattern.y /= roiKeypoints.size();

            std::cout << "Center of pattern: (" << cenScenePattern.x << ", " << cenScenePattern.y << ")\n";

            cv::circle( procesedScene, cenScenePattern, std::max(patternSize.width, patternSize.height)/4, cv::Scalar( 0, 255, 5 ), 2, 1 );

            cv::destroyWindow("Warped Pattern found_"+m);
            cv::destroyWindow("Pattern found_"+m);
        }
    } else {
        procesedScene = scene.getImage();
    }
    cv::imshow("Input", procesedScene);
    cv::waitKey();
    return true;
}

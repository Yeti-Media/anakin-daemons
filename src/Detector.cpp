#include <opencv2/opencv.hpp>
#include <Detector.hpp>

#define HOMOGRAPHY_REPROJECTION_THRESHOLD 3.0f
#define MIN_MATCHES_ALLOWED 8

using namespace Anakin;

Detector::Detector(cv::Ptr<cv::DescriptorMatcher> detector,  std::vector<RichImg>& patterns){
    this->detector = detector;
    this->patterns = &patterns;
}

void Detector::init() {
    for (int p = 0; p < this->patterns->size(); p++) {
        RichImg* pattern = &(*(this->patterns))[p];
        std::vector<cv::Mat> descriptors(1);
        descriptors[0] = pattern->getDescriptors().clone();
        this->detector->add(descriptors);
    }
    this->detector->train();
}

void Detector::getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches) {
    matches.clear();
    // To avoid NaN's when best match has zero distance we will use inversed ratio.
    const float minRatio = 1.f / 1.5f;
    std::vector< std::vector<cv::DMatch> > m_knnMatches;
    // KNN match will return 2 nearest matches for each query descriptor
    this->detector->knnMatch(queryDescriptors, m_knnMatches, 2);
    for (size_t i=0; i<m_knnMatches.size(); i++) {
        if (m_knnMatches[i].empty()) continue;
        const cv::DMatch& bestMatch   = m_knnMatches[i][0];
        const cv::DMatch& betterMatch = m_knnMatches[i][1];
        float distanceRatio = bestMatch.distance / betterMatch.distance;
        // Pass only matches where distance ratio between
        // nearest matches is greater than 1.5 (distinct criteria)
        if (distanceRatio < minRatio) {
            matches.push_back(bestMatch);
        }
    }
    //this->detector->match(queryDescriptors, matches);
}

std::vector<Match>* Detector::findPatterns(RichImg* scene) {
    std::vector<Match>* result = new std::vector<Match>();
    for (int p = 0; p < this->patterns->size(); p++) {
        Match* match;
        RichImg* pattern = &(*(this->patterns))[p];
        if (findPattern(scene, pattern, &match)) {
            result->push_back(*match);
        }
    }
    return result;
}


bool refineMatchesWithHomography(
                                const std::vector<cv::KeyPoint>& queryKeypoints,
                                const std::vector<cv::KeyPoint>& trainKeypoints,
                                float reprojectionThreshold,
                                std::vector<cv::DMatch>& matches,
                                cv::Mat& homography
    ) {

    if (matches.size() < MIN_MATCHES_ALLOWED)
        return false;

    // Prepare data for cv::findHomography
    std::vector<cv::Point2f> srcPoints(matches.size());
    std::vector<cv::Point2f> dstPoints(matches.size());

    for (size_t i = 0; i < matches.size(); i++) {
        srcPoints[i] = trainKeypoints[matches[i].trainIdx].pt;
        dstPoints[i] = queryKeypoints[matches[i].queryIdx].pt;
    }

    // Find homography matrix and get inliers mask
    std::vector<unsigned char> inliersMask(srcPoints.size());
    homography = cv::findHomography(srcPoints,
                                    dstPoints,
                                    CV_FM_RANSAC,
                                    HOMOGRAPHY_REPROJECTION_THRESHOLD,
                                    inliersMask);

    std::vector<cv::DMatch> inliers;
    for (size_t i=0; i<inliersMask.size(); i++) {
        if (inliersMask[i])
            inliers.push_back(matches[i]);
    }

    matches.swap(inliers);
    return matches.size() > MIN_MATCHES_ALLOWED;
}



bool Detector::findPattern(RichImg* scene, RichImg* pattern, Match** match) {
    std::vector<cv::DMatch>* matches = new std::vector<cv::DMatch>();
    cv::Mat homography;
    cv::Mat homography_refined;
    cv::Mat homography_final;
    cv::Mat warpedSceneMat;
    // Get matches with current pattern
    getMatches(scene->getDescriptors(), *matches); //<== current crash pos

    // Find homography transformation and detect good matches
    bool homographyFound = refineMatchesWithHomography(
        scene->getKeypoints(),
        pattern->getKeypoints(),
        HOMOGRAPHY_REPROJECTION_THRESHOLD,
        *matches,
        homography);

    if (homographyFound) {

        // Warp image using found homography
        cv::warpPerspective(scene->getImage()->getGrayImg(),
                            warpedSceneMat,
                            homography,
                            pattern->getImage()->getSize(),
                            cv::WARP_INVERSE_MAP | cv::INTER_CUBIC);

        std::vector<cv::DMatch>* refinedMatches = new std::vector<cv::DMatch>();;
        Img* warpedSceneImg = new Img(warpedSceneMat);
        RichImg* warpedScene = scene->makeNew(warpedSceneImg);

        // Match with pattern
        getMatches(warpedScene->getDescriptors(), *refinedMatches);

        // Estimate new refinement homography

        homographyFound = refineMatchesWithHomography(
                warpedScene->getKeypoints(),
                pattern->getKeypoints(),
                HOMOGRAPHY_REPROJECTION_THRESHOLD,
                *refinedMatches,
                homography_refined);

        // Get a result homography as result of matrix product of refined and rough homographies:
        if (homographyFound) {
            homography_final = homography * homography_refined;
            *match = new Match(scene, pattern, refinedMatches, homography_final);
        }

        delete warpedScene;
    }

    return homographyFound;
}

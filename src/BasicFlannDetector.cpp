#include "BasicFlannDetector.hpp"
#include <time.h> // for benchmark
#include <boost/foreach.hpp>
#define DEBUG 0


using namespace Anakin;
using namespace cv;
using namespace std;

    void BasicFlannDetector::init() {
        std::vector<cv::Mat> pdescriptors(this->patterns->size());
        for (int i = 0; i < this->patterns->size();i++){
            RichImg* p = this->patterns->at(i);
            pdescriptors.at(i) = p->getDescriptors().clone();
        }
        this->detector->add(pdescriptors);
        this->detector->train();
    }

    BasicFlannDetector::BasicFlannDetector( cv::Ptr<SerializableFlannBasedMatcher/*cv::DescriptorMatcher*/>  detector,  std::vector<Anakin::RichImg*>& patterns, bool train, float minRatio, int min_matches_allowed) {
        this->detector = detector;
        this->patterns = &patterns;
        this->minRatio = minRatio;
        this->min_matches_allowed = min_matches_allowed;
        this->train = train;
//        if (this->train) {
//            init();
//        }
    }

    int testPointWithLine(Point2f p1, Point2f p2, Point2f point) {
        float x1 = p1.x;
        float y1 = p1.y;
        float x2 = p2.x;
        float y2 = p2.y;
        float x = point.x;
        float y = point.y;
        float y2my1 = y2 - y1;
        float x2mx1 = x2 - x1;
        float xmx1 = x - x1;
        float result = y - y1 - ((y2my1/x2mx1) * xmx1);
        return result;
    }

    void BasicFlannDetector::cleanMatchedKeypoints(Match* match) {
    #if DEBUG
        cv::Mat keypointsMat;
        cv::namedWindow("Keypoints", CV_WINDOW_AUTOSIZE);
        cv::drawKeypoints(  match->getScene()->getImage()->getImage(),
                            match->getScene()->getKeypoints(),
                            keypointsMat, cv::Scalar(255,0,0),
                            cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        cv::imshow("Keypoints", keypointsMat);
        cv::waitKey();
        cv::destroyWindow("Keypoints");
    #endif
        std::vector<cv::KeyPoint> keypoints = match->getScene()->getKeypoints();
        std::vector<int> mask(keypoints.size());
        std::vector<DMatch>* matches = match->getMatches();

        int removedKeypoints = 0;
        for (uint m = 0; m < match->getMatches()->size(); m++) {
            DMatch cmatch = (*matches)[m];
            mask[cmatch.trainIdx] = 1;
            removedKeypoints++;
        }


        Mat img_object = match->getPattern()->getImage()->getImage();
        std::vector<Point2f> obj_corners(4);
        obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
        obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
        std::vector<Point2f> scene_corners(4);

        perspectiveTransform( obj_corners, scene_corners, match->getHomography());

        Point2f p1 = scene_corners[0]; //upper left

        Point2f p2 = scene_corners[1]; //upper right

        Point2f p3 = scene_corners[2]; //bottom right

        Point2f p4 = scene_corners[3]; //bottom left

        //int removedKeypoints = 0;
        for (uint k = 0; k < keypoints.size(); k++) {
            KeyPoint kp = keypoints[k];
            Point2f kpoint = kp.pt;
            bool test1 = testPointWithLine(p1,p2, kpoint) == 0 || (testPointWithLine(p1,p2, kpoint)*testPointWithLine(p1,p2, p3)) >= 0;
            bool test2 = testPointWithLine(p2,p3, kpoint) == 0 || (testPointWithLine(p2,p3, kpoint)*testPointWithLine(p2,p3, p1)) >= 0;
            bool test3 = testPointWithLine(p3,p4, kpoint) == 0 || (testPointWithLine(p3,p4, kpoint)*testPointWithLine(p3,p4, p1)) >= 0;
            bool test4 = testPointWithLine(p1,p4, kpoint) == 0 || (testPointWithLine(p1,p4, kpoint)*testPointWithLine(p1,p4, p3)) >= 0;
            if (test1 && test2 && test3 && test4) {
                mask[k] = 1;
                removedKeypoints++;
            }
        }
        //std::cout << "removed keypoints : " << removedKeypoints << "\n";
        match->getScene()->recalculateFeatures(mask);


    #if DEBUG

        cv::namedWindow("Keypoints", CV_WINDOW_AUTOSIZE);
        cv::drawKeypoints(  match->getScene()->getImage()->getImage(),
                            match->getScene()->getKeypoints(),
                            keypointsMat, cv::Scalar(255,0,0),
                            cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

        line( keypointsMat, p1, p2, Scalar( 0, 255, 0), 4 );
        line( keypointsMat, p2, p3, Scalar( 0, 255, 0), 4 );
        line( keypointsMat, p3, p4, Scalar( 0, 255, 0), 4 );
        line( keypointsMat, p4, p1, Scalar( 0, 255, 0), 4 );


        cv::imshow("Keypoints", keypointsMat);
        cv::waitKey();
        cv::destroyWindow("Keypoints");
    #endif
    }

    void BasicFlannDetector::cleanMatchedKeypoints(Match* match, std::vector<bool>* mask) {
        std::vector<cv::KeyPoint> keypoints = match->getScene()->getKeypoints();
        std::vector<DMatch>* matches = match->getMatches();
        int removedKeypoints = 0;
        for (uint m = 0; m < match->getMatches()->size(); m++) {
            DMatch cmatch = (*matches)[m];
            mask->at(cmatch.trainIdx) = false;
            removedKeypoints++;
        }
    }


    std::vector<Match>* BasicFlannDetector::findPatterns(RichImg* scene) {
        if (this->train) {
            return findPatterns_usingTraining(scene);
        }
        std::vector<Match>* result = new std::vector<Match>();
        bool reRun = true;
        std::vector<bool>* mask = new std::vector<bool>(scene->getKeypoints().size(), true);
        while(reRun) {
            reRun = false;
            for (uint p = 0; p < this->patterns->size(); p++) {
                Match* match;
                RichImg* pattern = (*(this->patterns))[p];
                if (findPattern(scene, pattern, &match, mask)) {
                    result->push_back(*match);
                    cleanMatchedKeypoints(match, mask);
                    reRun = true;
                }
            }
        }

        return result;
    }

    void BasicFlannDetector::getMatches(const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches) {
        std::vector< std::vector<cv::DMatch> > m_knnMatches;
        if (queryDescriptors.empty()) return;
        if(queryDescriptors.type()!=CV_32F) {
            queryDescriptors.convertTo(queryDescriptors, CV_32F);
        }
        ((cv::Ptr<SerializableFlannBasedMatcher>)this->detector)->knnMatch(queryDescriptors, m_knnMatches, 2);
        std::cout << "knnMatch: " << m_knnMatches.size() << " matches" << std::endl;
        for (size_t i=0; i<m_knnMatches.size(); i++) {
            if (m_knnMatches[i].empty()) continue;
                const cv::DMatch& match_0 = m_knnMatches[i][0];
                const cv::DMatch& match_1 = m_knnMatches[i][1];
                if (match_0.distance < match_1.distance * this->minRatio) {
                    matches.push_back(match_0);
                }
        }
    }

    bool BasicFlannDetector::keyExist(map<int, vector<DMatch>*>* m, int key) {
        if ( m->find(key) == m->end() ) {
            return false;
        } else {
            return true;
        }
    }

    void BasicFlannDetector::getKeys(map<int, vector<DMatch>*>* m, vector<int>* keys) {
        pair<int,vector<DMatch>*> me; // what a map<int, int> is made of
        BOOST_FOREACH(me, *m) {
            keys->push_back(me.first);
        }
    }

    vector<Anakin::Match>* BasicFlannDetector::findPatterns_usingTraining(Anakin::RichImg* scene) {
        vector<Match>* result = new std::vector<Match>();
        vector<DMatch>* good_matches = new vector<DMatch>(0);
        getMatches(scene->getDescriptors(), *good_matches);
        map<int, vector<DMatch>*>* patternsMatched = new map<int, vector<DMatch>*>();
        for (uint m = 0; m < good_matches->size(); m++) {
            DMatch match = good_matches->at(m);
            vector<DMatch>* pmatches;
            if (keyExist(patternsMatched, match.imgIdx)) {
                pmatches = patternsMatched->find(match.imgIdx)->second;
            } else {
                pmatches = new vector<DMatch>(0);
                (*patternsMatched)[match.imgIdx] = pmatches;
            }
            pmatches->push_back(match);
        }
        vector<int>* matchedImgsIdx = new vector<int>(0);
        getKeys(patternsMatched, matchedImgsIdx);
        for (uint pm = 0; pm < matchedImgsIdx->size(); pm++) {
            int currentKey = matchedImgsIdx->at(pm);
            vector<DMatch>* pattern_matches;
            if (keyExist(patternsMatched, currentKey)) {
                pattern_matches = patternsMatched->find(currentKey)->second;
            } else {
                continue;
            }
            if (pattern_matches->size() >= this->min_matches_allowed) {
                std::vector<Point2f> obj_points;
                std::vector<Point2f> scene_points;

                RichImg* pattern = this->patterns->at(currentKey);

                for( uint i = 0; i < pattern_matches->size(); i++ ) {
                    //-- Get the keypoints from the good matches
                    //obj_points.push_back( pattern->getKeypoints()[ (*pattern_matches)[i].queryIdx ].pt );
                    //scene_points.push_back( scene->getKeypoints()[ (*pattern_matches)[i].trainIdx ].pt );
                    obj_points.push_back( pattern->getKeypoints()[ (*pattern_matches)[i].trainIdx ].pt );
                    scene_points.push_back( scene->getKeypoints()[ (*pattern_matches)[i].queryIdx ].pt );
                }

                std::vector<cv::KeyPoint>* matchedKeypoints = new std::vector<cv::KeyPoint>(0);
                for (uint m = 0; m < pattern_matches->size(); m++) {
                    DMatch cmatch = (*pattern_matches)[m];
                    //matchedKeypoints->push_back(scene->getKeypoints()[cmatch.trainIdx]);
                    matchedKeypoints->push_back(scene->getKeypoints()[cmatch.queryIdx]);
                }

                cv::Point2f center(0,0);
                for ( size_t i=0; i<matchedKeypoints->size(); i++ ) {
                    cv::KeyPoint currentPoint = (*matchedKeypoints)[i];
                    center.x += currentPoint.pt.x;
                    center.y += currentPoint.pt.y;
                }
                center.x /= matchedKeypoints->size();
                center.y /= matchedKeypoints->size();

                Mat H = findHomography( obj_points, scene_points, CV_RANSAC );
                Match* pmatch = new Match(scene, pattern, pattern_matches, H, center, *matchedKeypoints);
                result->push_back(*pmatch);
            }
        }
        return result;
    }

    void BasicFlannDetector::getMatches(const cv::Mat& patternDescriptors, const cv::Mat& queryDescriptors, std::vector<cv::DMatch>& matches, std::vector<bool>* mask) {
        std::vector< std::vector<cv::DMatch> > m_knnMatches;
        if (patternDescriptors.empty() || queryDescriptors.empty()) return;
        if(patternDescriptors.type()!=CV_32F) {
            patternDescriptors.convertTo(patternDescriptors, CV_32F);
        }
        if(queryDescriptors.type()!=CV_32F) {
            queryDescriptors.convertTo(queryDescriptors, CV_32F);
        }
        this->detector->knnMatch(patternDescriptors, queryDescriptors, m_knnMatches, 2);
        for (size_t i=0; i<m_knnMatches.size(); i++) {
            if (m_knnMatches[i].empty()) continue;
            const cv::DMatch& bestMatch = m_knnMatches[i][0];
            const cv::DMatch& betterMatch = m_knnMatches[i][1];
            float distanceRatio = bestMatch.distance / betterMatch.distance;
            if (distanceRatio < this->minRatio) {
                if (!mask->at(bestMatch.trainIdx)) {
                    continue;
                }
                matches.push_back(bestMatch);
            }
        }

    }

    bool BasicFlannDetector::refineMatchesWithHomography(
                                const std::vector<cv::KeyPoint>& queryKeypoints,
                                const std::vector<cv::KeyPoint>& trainKeypoints,
                                float reprojectionThreshold,
                                std::vector<cv::DMatch>& matches,
                                cv::Mat& homography
    ) {
        return true;
    }

    bool BasicFlannDetector::findPattern(RichImg* scene, RichImg* pattern, Match** match, std::vector<bool>* mask) {
        std::vector<DMatch>* good_matches = new std::vector<DMatch>(0);
        getMatches(pattern->getDescriptors(), scene->getDescriptors(), *good_matches, mask);

#if DEBUG
        std::cout << "matches : " << good_matches->size() << "\n";

        Mat img_matches;
        drawMatches( pattern->getImage()->getImage(), pattern->getKeypoints(), scene->getImage()->getImage(), scene->getKeypoints(),
                    *good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                    vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        imshow( "Matches", img_matches );

        waitKey();

#endif
        if (good_matches->size() >= this->min_matches_allowed) {
            std::vector<Point2f> obj_points;
            std::vector<Point2f> scene_points;

            for( uint i = 0; i < good_matches->size(); i++ ) {
                //-- Get the keypoints from the good matches
                obj_points.push_back( pattern->getKeypoints()[ (*good_matches)[i].queryIdx ].pt );
                scene_points.push_back( scene->getKeypoints()[ (*good_matches)[i].trainIdx ].pt );
            }

            std::vector<cv::KeyPoint>* matchedKeypoints = new std::vector<cv::KeyPoint>(0);
            for (uint m = 0; m < good_matches->size(); m++) {
                DMatch cmatch = (*good_matches)[m];
                matchedKeypoints->push_back(scene->getKeypoints()[cmatch.trainIdx]);
            }

            cv::Point2f center(0,0);
            for ( size_t i=0; i<matchedKeypoints->size(); i++ ) {
                cv::KeyPoint currentPoint = (*matchedKeypoints)[i];
                center.x += currentPoint.pt.x;
                center.y += currentPoint.pt.y;
            }
            center.x /= matchedKeypoints->size();
            center.y /= matchedKeypoints->size();

            Mat H = findHomography( obj_points, scene_points, CV_RANSAC );
            *match = new Match(scene, pattern, good_matches, H, center, *matchedKeypoints);
        } else return false;

        return true;
    }

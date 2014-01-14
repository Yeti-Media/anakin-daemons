#ifndef STEINNORMALIZER_HPP
#define STEINNORMALIZER_HPP

#include <opencv2/opencv.hpp>
#include <Barrier.hpp>

namespace Anakin {

class SteinNormalizer {
    public:
        SteinNormalizer(cv::Mat img, int segmentSize);
        SteinNormalizer(); //used for testing, to be removed later
        cv::Mat normalize();
    protected:
    private:
        //FUNCTIONS
        std::vector<cv::Point>* getNeighbors(const cv::Point pos);
        int getMaxLabel(std::vector<cv::Point>* neighbors);
        std::vector<int>* getSegmentValues(const int value);
        int getMean(std::vector<int>* values);
        bool areInSameSegment(const int v1, const int v2);
        //FIELDS
        cv::Mat sourceImg;
        cv::Mat labels;
        std::vector<std::vector<int>*>* segmentsValues;
        int segments;
        int segmentSize;
        //DEBUG FUNCTIONS AND FIELDS
        void printSegments(std::vector<std::vector<int>*>* segments);
        void printMeanPerValue(std::vector<std::vector<int>*>* segments);

};

};

#endif // STEINNORMALIZER_HPP

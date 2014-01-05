#ifndef STEINNORMALIZER_HPP
#define STEINNORMALIZER_HPP

#include <opencv2/opencv.hpp>
#include <Barrier.hpp>

namespace Anakin {

class SteinNormalizer {
    public:
        SteinNormalizer(cv::Mat, int segments);
        SteinNormalizer(); //used for testing, to be removed later
        cv::Mat normalize(int threadsToRun=4);
        struct ComponentArg {
            public:
                ComponentArg(int n) {
                    test = new std::vector<int>(n,0);
                    barrier = new Barrier(n);
                }
                int get(int idx) {
                    return test->at(idx);
                }
                void set(int idx, int value) {
                    test->at(idx) = value;
                }
                int getSize() {
                    return test->size();
                }
                Barrier* barrier;
            private:
                std::vector<int>* test;
        };
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
        std::vector<std::vector<int>> segmentsValues;
        int segments;

};

};

#endif // STEINNORMALIZER_HPP

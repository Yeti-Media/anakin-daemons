#ifndef IMAGECOMPONENTEXTRACTOR_HPP
#define IMAGECOMPONENTEXTRACTOR_HPP

#include <opencv2/opencv.hpp>

namespace Anakin {

class ImageComponentExtractor {
    public:
        ImageComponentExtractor(cv::Mat image);
        void demo_contours(int mode=CV_CHAIN_APPROX_NONE, int threshold_begin=0, int threshold_end=256, int thresh_mode=CV_THRESH_BINARY);
        void demo_edges(double threshold, int apertureSize=3, bool L2gradient=false, int ratio=3, bool useBlur=false);
        void demo_contours_using_edges(
                                        int mode,
                                        int threshold_begin,
                                        int threshold_end,
                                        int thresh_mode,
                                        double threshold,
                                        int apertureSize,
                                        bool L2gradient,
                                        int ratio,
                                        bool useBlur);
    protected:
    private:
        cv::Mat image;
};

};

#endif // IMAGECOMPONENTEXTRACTOR_HPP

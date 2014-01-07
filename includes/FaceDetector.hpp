#ifndef FACEDETECTOR_HPP
#define FACEDETECTOR_HPP

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <FaceMatch.hpp>

namespace Anakin {

class FaceDetector {
    public:
        FaceDetector(std::string mainCascadeClassifierData);
        FaceDetector(std::string mainCascadeClassifierData, std::vector<std::string>* detailsCascadeClassifierData);
        std::vector<Anakin::FaceMatch*>* detect(cv::Mat image, std::vector<cv::Rect>* mainDetections, std::vector<std::vector<cv::Rect>*>* details, bool lookForDetails = true);
        void showDetections(cv::Mat image, std::vector<Anakin::FaceMatch*>* matches);
    protected:
    private:
        void detect(cv::Mat image, std::vector<cv::Rect>* mainDetections);
        void detect(cv::Mat image, cv::CascadeClassifier* classifier, std::vector<cv::Rect>* detections);
        void load(std::string file, cv::CascadeClassifier* classifier);
        cv::CascadeClassifier* mainClassifier;
        std::vector<cv::CascadeClassifier*>* detailsClassifiers;
        bool forceIgnoreDetails=false;
        std::string mainLabel;
        std::vector<std::string>* detailsLabels;
        std::string getFilename (const std::string& str);
};

};

#endif // FACEDETECTOR_HPP

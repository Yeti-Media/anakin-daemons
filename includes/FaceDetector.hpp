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
        void setScaleFactor(float value);
        void setMinNeighbors(int value);
        void setMinSize(cv::Size value);
        void setMaxSize(cv::Size value);
        float getScaleFactor();
        int getMinNeighbors();
        cv::Size getMinSize();
        cv::Size getMaxSize();
    protected:
    private:
        void detect(cv::Mat image, std::vector<cv::Rect>* mainDetections);
        void detect(cv::Mat image, cv::CascadeClassifier* classifier, std::vector<cv::Rect>* detections);
        void detect_default(cv::Mat image, cv::CascadeClassifier* classifier, std::vector<cv::Rect>* detections);
        void load(std::string file, cv::CascadeClassifier* classifier);
        cv::CascadeClassifier* mainClassifier;
        std::vector<cv::CascadeClassifier*>* detailsClassifiers;
        bool forceIgnoreDetails=false;
        std::string mainLabel;
        std::vector<std::string>* detailsLabels;
        std::string getFilename (const std::string& str);
        float scaleFactor = 1.1;
        int minNeighbors = 3;
        cv::Size minSize = cv::Size();
        cv::Size maxSize = cv::Size();
};

};

#endif // FACEDETECTOR_HPP

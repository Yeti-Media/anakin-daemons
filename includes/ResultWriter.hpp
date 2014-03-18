#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include <JSON.h>
#include <JSONValue.h>
#include <opencv2/opencv.hpp>
#include <HistMatch.hpp>
#include <FaceMatch.hpp>

using namespace cv;
using namespace std;
namespace Anakin {

class ResultWriter {

    public:

        ResultWriter() {};

        std::wstring output(char mode, std::string data, char colors=0);
        JSONValue* resultAsJSONValue(char mode, std::string data, char colors=0);
        static const char RW_PATTERNS = 1;
        static const char RW_HISTOGRAMS = 2;
        static const char RW_LANDSCAPE = 4;

        static const char RW_COLOR = 8;
        static const char RW_GRAY = 16;
        static const char RW_HSV = 32;

        static const std::string RW_PATTERN_MATCHING;
        static const std::string RW_HISTOGRAM_MATCHING;
        static const std::string RW_LANDSCAPE_MATCHING;
        static const std::string RW_OCR;
        static const std::string RW_FACE_DETECTION;

        wstring outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

        wstring outputResult(string label, vector<JSONValue*> values);

        wstring outputResult(vector<HistMatch*>* histMatches);

        wstring outputResult(vector<string>* ocrRecognizedText);

        wstring outputResult(vector<FaceMatch*>* matches);

        wstring outputResult(string requestID, string category, vector<JSONValue*> jsonValues);

        JSONValue* resultAsJSONValue(string requestID, string category, vector<JSONValue*> jsonValues);

        JSONValue* resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

        JSONValue* resultAsJSONValue(string label, vector<JSONValue*> values);

        JSONValue* resultAsJSONValue(vector<Anakin::HistMatch*>* histMatches);

        JSONValue* resultAsJSONValue(vector<string>* ocrRecognizedText);

        JSONValue* resultAsJSONValue(vector<FaceMatch*>* matches);

        JSONValue* resultAsJSONValue(FaceMatch* match);

};
};


#endif // RESULTWRITER_HPP_INCLUDED

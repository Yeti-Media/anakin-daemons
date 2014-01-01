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

    wstring outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

    wstring outputResult(string label, vector<JSONValue*> values);

    wstring outputResult(vector<HistMatch*>* histMatches);

    wstring outputResult(vector<string>* ocrRecognizedText);

    wstring outputResult(vector<FaceMatch*>* matches);

    JSONValue* resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

    JSONValue* resultAsJSONValue(string label, vector<JSONValue*> values);

    JSONValue* resultAsJSONValue(vector<Anakin::HistMatch*>* histMatches);

    JSONValue* resultAsJSONValue(vector<string>* ocrRecognizedText);

    JSONValue* resultAsJSONValue(vector<FaceMatch*>* matches);

    JSONValue* resultAsJSONValue(FaceMatch* match);

}


#endif // RESULTWRITER_HPP_INCLUDED

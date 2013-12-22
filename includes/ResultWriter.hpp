#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include <JSON.h>
#include <JSONValue.h>
#include <opencv2/opencv.hpp>
#include <HistMatch.hpp>

using namespace cv;
using namespace std;
namespace Anakin {

    wstring outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

    wstring outputResult(string label, vector<JSONValue*> values);

    wstring outputResult(vector<HistMatch*>* histMatches);

    JSONValue* resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

    JSONValue* resultAsJSONValue(string label, vector<JSONValue*> values);

    JSONValue* resultAsJSONValue(vector<Anakin::HistMatch*>* histMatches);

}


#endif // RESULTWRITER_HPP_INCLUDED

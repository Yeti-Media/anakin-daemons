#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include <JSON.h>
#include <JSONValue.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
namespace Anakin {

    wstring outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

    JSONValue* resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

}


#endif // RESULTWRITER_HPP_INCLUDED

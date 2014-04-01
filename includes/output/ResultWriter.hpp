#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include "output/JSON.h"
#include "output/JSONValue.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
namespace Anakin {

class ResultWriter {

    public:

        ResultWriter() {};

        static const std::string RW_PATTERN_MATCHING;
        static const std::string RW_CACHE_IDX_ADD;
        static const std::string RW_CACHE_IDX_DEL;
        static const std::string RW_CACHE_IDX_UPD;
        static const std::string RW_CACHE_IDX_STATUS;

        wstring outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

        wstring outputResult(string label, vector<JSONValue*> values);

        wstring outputResult(string requestID, string category, vector<JSONValue*> jsonValues);

        JSONValue* resultAsJSONValue(string requestID, string category, vector<JSONValue*> jsonValues);

        JSONValue* resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints);

        JSONValue* resultAsJSONValue(string label, vector<JSONValue*> values);

        JSONValue* resultAsJSONValue(int smatcher_id_added, int cacheFreeSpace, int smatcher_id_removed);

        JSONValue* resultAsJSONValue(int smatcher_id_deleted, int cacheFreeSpace);

        JSONValue* resultAsJSONValue(int smatcher_id_updated);

        JSONValue* resultAsJSONValue(vector<int> smatchers_in_cache, int cacheFreeSpace);

        JSONValue* parseJSON(const char * data);

        std::string jsonReqToString(JSONValue* req);

};
};


#endif // RESULTWRITER_HPP_INCLUDED

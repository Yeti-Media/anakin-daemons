#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;


wstring Anakin::outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    return resultAsJSONValue(center, label, matchedKeypoints)->Stringify().c_str();
}

JSONValue* Anakin::resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    /*  Result as JSONObject

        root    -> center   -> x (float)
                            -> y (float)

                -> label (string)

                -> keypoints (JSONArray)    -> pos      -> x (float)
                                                        -> y (float)

                                            -> angle (float)

                                            -> size (float)

                                            -> response (float)
    */
    JSONObject root;
    JSONObject jcenter;
    JSONArray keypoints;

    jcenter[L"x"] = new JSONValue(center.x);
    jcenter[L"y"] = new JSONValue(center.y);

	// Adding a string
	root[L"center"] = new JSONValue(jcenter);
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());


	for (int k = 0; k < matchedKeypoints.size(); k++) {
        KeyPoint current = matchedKeypoints[k];
        JSONObject keypoint;
        JSONObject pos;
        pos[L"x"] = new JSONValue(current.pt.x);
        pos[L"y"] = new JSONValue(current.pt.y);
        keypoint[L"pos"] = new JSONValue(pos);
        keypoint[L"angle"] = new JSONValue(current.angle);
        keypoint[L"size"] = new JSONValue(current.size);
        keypoint[L"response"] = new JSONValue(current.response);
		keypoints.push_back(new JSONValue(keypoint));
    }
	root[L"keypoints"] = new JSONValue(keypoints);

	// Create a value
	JSONValue *value = new JSONValue(root);
	return value;
}


#include <ResultWriter.hpp>
#define LIGH_RESULTS 1

using namespace Anakin;
using namespace cv;
using namespace std;


const std::string ResultWriter::RW_PATTERN_MATCHING = "PATTERN";
const std::string ResultWriter::RW_CACHE_IDX_ADD = "INDEX ADD";
const std::string ResultWriter::RW_CACHE_IDX_DEL = "INDEX DELETE";
const std::string ResultWriter::RW_CACHE_IDX_UPD = "INDEX UPDATE";
const std::string ResultWriter::RW_CACHE_IDX_STATUS = "INDEX STATUS";

wstring ResultWriter::outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    return resultAsJSONValue(center, label, matchedKeypoints)->Stringify().c_str();
}

wstring ResultWriter::outputResult(string label, vector<JSONValue*> values) {
    return resultAsJSONValue(label, values)->Stringify().c_str();
}

wstring ResultWriter::outputResult(string requestID, string category, vector<JSONValue*> jsonValues) {
    return resultAsJSONValue(requestID, category, jsonValues)->Stringify().c_str();
}

JSONValue* ResultWriter::resultAsJSONValue(string requestID, string category, vector<JSONValue*> jsonValues) {
    /*  Result as JSONObject

        root    -> requestID (string)

                -> category (string)

                -> values (JSONArray)
    */
    JSONObject root;
    wstringstream ws;
    ws << requestID.c_str();
	root[L"requestID"] = new JSONValue(ws.str());
	wstringstream ys;
    ys << category.c_str();
    root[L"category"] = new JSONValue(ys.str());
	JSONArray values;
	for (uint v = 0; v < jsonValues.size(); v++) {
		values.push_back(jsonValues.at(v));
    }

	root[L"values"] = new JSONValue(values);

    JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    /*  Result as JSONObject

        root    -> center   -> x (float)
                            -> y (float)

                -> pattern label (string)

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


	for (uint k = 0; k < matchedKeypoints.size() && !LIGH_RESULTS; k++) {
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

JSONValue* ResultWriter::resultAsJSONValue(string label, vector<JSONValue*> jsonValues) {
    /*  Result as JSONObject

        root    -> scene label (string)

                -> values (JSONArray)    -> <see function above>
    */
    JSONObject root;
    wstringstream ws;
    ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray values;

	for (uint v = 0; v < jsonValues.size(); v++) {
		values.push_back(jsonValues.at(v));
    }

	root[L"values"] = new JSONValue(values);

    JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(string smatcher_id_added, int cacheFreeSpace, string smatcher_id_removed) {
    /*  Result as JSONObject

        root    -> index_added (string)

                -> index_removed (string)

                -> cache_free_space (int)
    */
    JSONObject root;
    wstringstream ws;
    wstringstream xs;
    ws << smatcher_id_added.c_str();
    xs << smatcher_id_removed.c_str();
    root[L"index_added"] = new JSONValue(ws.str());
    root[L"index_removed"] = new JSONValue(xs.str());
    root[L"cache_free_space"] = new JSONValue((double)cacheFreeSpace);
    JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(string smatcher_id_deleted, int cacheFreeSpace) {
    return resultAsJSONValue("", cacheFreeSpace, smatcher_id_deleted);
}

JSONValue* ResultWriter::resultAsJSONValue(string smatcher_id_updated) {
    /*  Result as JSONObject

        root    -> index_updated (string)
    */
    JSONObject root;
    wstringstream ws;
    ws << smatcher_id_updated.c_str();
    root[L"index_updated"] = new JSONValue(ws.str());
    JSONValue *value = new JSONValue(root);
	return value;
}

JSONValue* ResultWriter::resultAsJSONValue(vector<string> smatchers_in_cache, int cacheFreeSpace) {
    /*  Result as JSONObject

        root    -> cache_free_space (int)

                -> indexes (JSONArray)    -> index (string)
    */
    JSONObject root;
	root[L"cache_free_space"] = new JSONValue((double)cacheFreeSpace);
	JSONArray values;

	for (uint v = 0; v < smatchers_in_cache.size(); v++) {
        string smatcher = smatchers_in_cache.at(v);
        wstringstream ws;
        ws << smatcher.c_str();
		values.push_back(new JSONValue(ws.str()));
    }

	root[L"indexes"] = new JSONValue(values);

    JSONValue *value = new JSONValue(root);
	return value;
}


#include <ResultWriter.hpp>

using namespace Anakin;
using namespace cv;
using namespace std;


wstring Anakin::outputResult(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
    return resultAsJSONValue(center, label, matchedKeypoints)->Stringify().c_str();
}

wstring Anakin::outputResult(string label, vector<JSONValue*> values) {
    return resultAsJSONValue(label, values)->Stringify().c_str();
}

wstring Anakin::outputResult(vector<HistMatch*>* histMatches) {
    return resultAsJSONValue(histMatches)->Stringify().c_str();
}

JSONValue* Anakin::resultAsJSONValue(Point2f center, string label, vector<KeyPoint> matchedKeypoints) {
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

JSONValue* Anakin::resultAsJSONValue(string label, vector<JSONValue*> jsonValues) {
    /*  Result as JSONObject

        root    -> scene label (string)

                -> values (JSONArray)    -> <see function above>
    */
    JSONObject root;
    wstringstream ws;
    ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray values;

	for (int v = 0; v < jsonValues.size(); v++) {
		values.push_back(jsonValues.at(v));
    }

	root[L"values"] = new JSONValue(values);

    JSONValue *value = new JSONValue(root);
	return value;

	//wcout << root->Stringify().c_str() << "\n";
}

JSONValue* Anakin::resultAsJSONValue(vector<HistMatch*>* histMatches) {
    /*  Result as JSONObject

        root    -> matches (JSONArray)    -> scene label (string)
                                          -> pattern label (string)
                                          -> percentage (float)


    */
    JSONObject root;
	JSONArray matches;
	for (int v = 0; v < histMatches->size(); v++) {
        wstringstream ws; wstringstream wp;
		JSONObject matchJson;
        HistMatch* match = histMatches->at(v);
        ws << match->getScene()->getLabel().c_str();
        matchJson[L"scene"] = new JSONValue(ws.str());
        wp << match->getPattern()->getLabel().c_str();
        matchJson[L"pattern"] = new JSONValue(wp.str());
        matchJson[L"percentage"] = new JSONValue(match->getPercentage());
        matches.push_back(new JSONValue(matchJson));
    }

	root[L"matches"] = new JSONValue(matches);

    JSONValue *value = new JSONValue(root);

	return value;

	//wcout << root->Stringify().c_str() << "\n";
}


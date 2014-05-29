/*
 * CommunicationFormatterMatchingJSON.cpp
 *
 *  Created on: May 28, 2014
 *      Author: renx
 */

#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>

#define LIGHT_RESULTS 1 //TODO delete this and define a const in the appropriate place
namespace Anakin {

CommunicationFormatterMatchingJSON::CommunicationFormatterMatchingJSON() {
}

wstring I_CommunicationFormatterMatching::outputMatch(Point2f center,
		string label, vector<KeyPoint> matchedKeypoints) {
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

	for (uint k = 0; k < matchedKeypoints.size() && !LIGHT_RESULTS; k++) {
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
	if (!LIGHT_RESULTS)
		root[L"keypoints"] = new JSONValue(keypoints);

	// Create a value
	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

wstring I_CommunicationFormatterMatching::outputMatches(string label,
		vector<string *> values) {
	/*  Result as JSONObject

	 root    -> scene label (string)

	 -> values (JSONArray)    -> <see function above>
	 */
	JSONObject root;
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray valuesJSON;

	for (uint v = 0; v < values.size(); v++) {
		JSONValue *auxValue = new JSONValue(values.at(v));
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

CommunicationFormatterMatchingJSON::~CommunicationFormatterMatchingJSON() {
}

} /* namespace Anakin */

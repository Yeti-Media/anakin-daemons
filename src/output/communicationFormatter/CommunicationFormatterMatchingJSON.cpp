/*
 * CommunicationFormatterMatchingJSON.cpp
 *
 *  Created on: May 28, 2014
 *      Author: Renzo Bianchini
 */

#define LIGTH_RESULTS 1

#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>

namespace Anakin {

CommunicationFormatterMatchingJSON::CommunicationFormatterMatchingJSON() {
}

wstring* CommunicationFormatterMatchingJSON::outputMatch(Point2f center,
		string label, vector<KeyPoint> matchedKeypoints) {
	/*   Result as wstring representing a  JSONObject

	 root    -> center   -> x (float)
	 -> y (float)

	 -> pattern label (string)

	 -> keypoints (JSONArray)    -> pos      -> x (float)
	 -> y (float)

	 -> angle (float)

	 -> size (float)

	 -> response (float)
	 */

	cout << "CommunicationFormatterMatchingJSON::outputMatch 38" << endl;

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

#if !LIGTH_RESULTS
	for (uint k = 0; k < matchedKeypoints.size(); k++) {
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
#endif //LIGTH_RESULTS

	// Create a value
	JSONValue *value = new JSONValue(root);
	return new wstring(value->Stringify().c_str());
}

wstring* CommunicationFormatterMatchingJSON::outputMatches(string label,
		vector<wstring *> values) {
	/*   Result as wstring representing a JSONObject

	 root    -> scene label (string)

	 -> values (JSONArray)    -> <see function above>
	 */

	cout << "CommunicationFormatterMatchingJSON::outputMatches 85" << endl;

	JSONObject root;
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray valuesJSON;

	for (uint v = 0; v < values.size(); v++) {
		JSONValue *auxValue = JSON::Parse(
				(const wchar_t *) values.at(v)->c_str());
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	JSONValue *value = new JSONValue(root);
	return new wstring(value->Stringify().c_str());
}

CommunicationFormatterMatchingJSON::~CommunicationFormatterMatchingJSON() {
}

} /* namespace Anakin */

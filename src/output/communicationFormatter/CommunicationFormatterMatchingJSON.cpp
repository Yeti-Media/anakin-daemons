/*
 * CommunicationFormatterMatchingJSON.cpp
 *
 *  Created on: May 28, 2014
 *      Author: Renzo Bianchini
 */

#include <output/communicationFormatter/CommunicationFormatterMatchingJSON.hpp>
#include <boost/locale.hpp>

namespace Anakin {

CommunicationFormatterMatchingJSON::CommunicationFormatterMatchingJSON() {
}

wstring* CommunicationFormatterMatchingJSON::outputMatch(const Point2f & center,
		const string & label, const vector<KeyPoint> & matchedKeypoints) {
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

	JSONObject root;
	JSONObject jcenter;
	JSONArray keypoints;

	jcenter[L"x"] = new JSONValue(center.x);
	jcenter[L"y"] = new JSONValue(center.y);

	// Adding a string
	root[L"center"] = new JSONValue(jcenter);
	root[L"label"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(label));

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

	return new wstring(JSONValue(root).Stringify());
}

wstring* CommunicationFormatterMatchingJSON::outputMatches(const string & label,
		const vector<wstring *> & values) {
	/*   Result as wstring representing a JSONObject

	 root    -> scene label (string)

	 -> values (JSONArray)    -> <see function above>
	 */

	JSONObject root;
	root[L"label"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(label));
	JSONArray valuesJSON;

	for (uint v = 0; v < values.size(); v++) {
		JSONValue *auxValue = JSON::Parse(
				(const wchar_t *) values.at(v)->c_str());
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	return new wstring(JSONValue(root).Stringify());
}

CommunicationFormatterMatchingJSON::~CommunicationFormatterMatchingJSON() {
}

} /* namespace Anakin */

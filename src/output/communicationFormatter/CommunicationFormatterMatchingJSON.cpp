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

wstring CommunicationFormatterMatchingJSON::outputMatch(Point2f center,
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
	wstring salida = value->Stringify().c_str();
	wcout << "outputMatch" << value->Stringify().c_str() << endl;
	return value->Stringify().c_str();
}

wstring CommunicationFormatterMatchingJSON::outputMatches(string label,
		vector<wstring *> values) {
	/*  Result as JSONObject

	 root    -> scene label (string)

	 -> values (JSONArray)    -> <see function above>
	 */

//	wstring estrin;
//	for (uint ig = 0; ig < values.size(); ig++) {
//		cout << "values->at(ig)" << values.at(ig) << endl;
//		estrin = *(values.at(ig));
//		wcout << "values->at(ig) " << estrin<< endl;
//	}
	//VERRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
	JSONObject root;
	wstringstream ws;
	ws << label.c_str();
	root[L"label"] = new JSONValue(ws.str());
	JSONArray valuesJSON;

	for (uint v = 0; v < values.size(); v++) {
		wstring* auxiliar = values.at(v);
		wcout << "auxiliar outputMatches values" << /*auxiliar->c_str()*/ *values.at(v) << endl;
		//JSONValue *auxValue = new JSONValue(*auxiliar);/*/(*values.at(v));*/
		const wchar_t * lala = auxiliar->c_str();
		JSONValue *auxValue = JSON::Parse(lala);
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	JSONValue *value = new JSONValue(root);
	wstring estrin = value->Stringify().c_str();
	wcout << "outputMatches" << estrin << endl;
//	wcout << "value->Stringify().c_str()" << estrin << endl;
//	wcout << "outputMatches" << value->Stringify().c_str() << endl;
	return estrin;//value->Stringify().c_str();
}

CommunicationFormatterMatchingJSON::~CommunicationFormatterMatchingJSON() {
}

} /* namespace Anakin */

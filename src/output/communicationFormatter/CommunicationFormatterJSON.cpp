/*
 * CommunicationFormatterJSON.cpp
 *
 *  Created on: May 23, 2014
 *      Author: renx
 */

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>

using namespace std;
using namespace cv;
using namespace Anakin;

CommunicationFormatterJSON::CommunicationFormatterJSON() {
}

wstring I_CommunicationFormatter::outputResponse(string requestID,
		string category, vector<string *> values) {

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
	JSONArray valuesJSON;
	for (uint v = 0; v < values.size(); v++) {
		JSONValue *auxValue = new JSONValue(values.at(v));
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	JSONValue *returnValue = new JSONValue(root);
	return returnValue->Stringify().c_str();

}

wstring I_CommunicationFormatter::outputError(e_error errorType,
		std::string message, std::string origin) {

	/*  Result as JSONObject

	 root    -> error_type (string)

	 -> message (string)

	 -> origin (string)
	 */
	JSONObject root;
	wstringstream wmessage;
	wmessage << message.c_str();
	wstringstream worigin;
	worigin << origin.c_str();
	std::wstring werror_type;
	switch (errorType) {
	case RW_ERROR_TYPE_WARNING: {
		werror_type = L"WARNING";
		break;
	}
	case RW_ERROR_TYPE_ERROR: {
		werror_type = L"ERROR";
		break;
	}
	case RW_ERROR_TYPE_FATAL: {
		werror_type = L"FATAL";
		break;
	}
	}
	root[L"error_type"] = new JSONValue(werror_type);
	root[L"message"] = new JSONValue(wmessage.str());
	root[L"origin"] = new JSONValue(worigin.str());
	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

wstring I_CommunicationFormatter::format(const char * data) {
	return (JSON::Parse(data))->Stringify().c_str();
}

wstring I_CommunicationFormatter::format(char mode, string data, char colors){
	/*  Result as JSONObject

	 root    -> type ("pattern" | "histogram" | "landscape")
	 -> colors (only if type != "pattern")   ->  color (bool)
	 ->  gray (bool)
	 ->  hsv (bool)
	 -> dataType ("YML" | "XML")
	 -> data (string)
	 */
	static const char RW_PATTERNS = 1;
	static const char RW_HISTOGRAMS = 2;
	static const char RW_LANDSCAPE = 4;

	static const char RW_COLOR = 8;
	static const char RW_GRAY = 16;
	static const char RW_HSV = 32;


	JSONObject root;

	if (mode & RW_PATTERNS) {
		root[L"type"] = new JSONValue(L"pattern");
	} else if (mode & RW_HISTOGRAMS) {
		root[L"type"] = new JSONValue(L"histogram");
	} else if (mode & RW_LANDSCAPE) {
		root[L"type"] = new JSONValue(L"landscape");
	}

	if ((mode & RW_HISTOGRAMS) || (mode & RW_LANDSCAPE)) {
		JSONObject jcolors;
		bool color = colors & RW_COLOR;
		bool gray = colors & RW_GRAY;
		bool hsv = colors & RW_HSV;
		jcolors[L"color"] = new JSONValue(color);
		jcolors[L"gray"] = new JSONValue(gray);
		jcolors[L"hsv"] = new JSONValue(hsv);
		root[L"colors"] = new JSONValue(jcolors);
	}

	root[L"dataType"] = new JSONValue(L"YML");
	wstringstream ws;
	ws << data.c_str();
	root[L"data"] = new JSONValue(ws.str());

	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

CommunicationFormatterJSON::~CommunicationFormatterJSON() {
}


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
	return (JSON::Parse(data))->Stringify();
}

CommunicationFormatterJSON::~CommunicationFormatterJSON() {
}


/*
 * CommunicationFormatterJSON.cpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
 */

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <utils/Constants.hpp>

using namespace std;
using namespace cv;
using namespace Anakin;

CommunicationFormatterJSON::CommunicationFormatterJSON() {
}

wstring* CommunicationFormatterJSON::outputResponse(const string & requestID,
		e_category category, const vector<wstring *> & values) {

	/*  Result as wstring representing a JSONObject

	 root    -> requestID (string)

	 -> category (string)

	 -> values (JSONArray)
	 */

	JSONObject root;
	wstringstream ws;
	ws << requestID.c_str();
	root[L"requestID"] = new JSONValue(ws.str());
	switch (category) {
	case CF_PATTERN_MATCHING: {
		root[L"category"] = new JSONValue(L"PATTERN");
		break;
	}
	case CF_CACHE_IDX_ADD: {
		root[L"category"] = new JSONValue(L"INDEX ADD");
		break;
	}
	case CF_CACHE_IDX_DEL: {
		root[L"category"] = new JSONValue(L"INDEX DELETE");
		break;
	}
	case CF_CACHE_IDX_UPD: {
		root[L"category"] = new JSONValue(L"INDEX UPDATE");
		break;
	}
	case CF_CACHE_IDX_STATUS: {
		root[L"category"] = new JSONValue(L"INDEX STATUS");
		break;
	}
	case CF_OCR: {
		root[L"category"] = new JSONValue(L"OCR");
		break;
	}
	default: {
		root[L"category"] = new JSONValue(L"NONE---ERROR");
		break;
	}
	}
	JSONArray valuesJSON;
	for (uint v = 0; v < values.size(); v++) {
		JSONValue *auxValue = JSON::Parse(values.at(v)->c_str());
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	return new wstring(JSONValue(root).Stringify());

}

wstring* CommunicationFormatterJSON::outputError(e_error errorType,
		const string & message, const string & origin) {

	/*  Result as wstring representing a JSONObject

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
	case CF_ERROR_TYPE_WARNING: {
		werror_type = L"WARNING";
		break;
	}
	case CF_ERROR_TYPE_ERROR: {
		werror_type = L"ERROR";
		break;
	}
	case CF_ERROR_TYPE_FATAL: {
		werror_type = L"FATAL";
		break;
	}
	}
	root[L"error_type"] = new JSONValue(werror_type);
	root[L"message"] = new JSONValue(wmessage.str());
	root[L"origin"] = new JSONValue(worigin.str());
	return new wstring(JSONValue(root).Stringify());
}

wstring* CommunicationFormatterJSON::format(const char * data) {

	return new wstring((JSON::Parse(data))->Stringify());
}

wstring* CommunicationFormatterJSON::format(e_mode mode, string data,
		e_color colors) {
	/*  Result as wstring representing a JSONObject

	 root    -> type ("pattern" | "histogram" | "landscape")
	 -> colors (only if type != "pattern")   ->  color (bool)
	 ->  gray (bool)
	 ->  hsv (bool)
	 -> dataType ("YML" | "XML")
	 -> data (string)
	 */

	JSONObject root;
	if (mode & CF_PATTERNS) {
		root[L"type"] = new JSONValue(L"pattern");
	} else if (mode & CF_HISTOGRAMS) {
		root[L"type"] = new JSONValue(L"histogram");
	} else if (mode & CF_LANDSCAPE) {
		root[L"type"] = new JSONValue(L"landscape");
	}

	if ((mode & CF_HISTOGRAMS) || (mode & CF_LANDSCAPE)) {
		JSONObject jcolors;
		bool color = colors & CF_COLOR;
		bool gray = colors & CF_GRAY;
		bool hsv = colors & CF_HSV;
		jcolors[L"color"] = new JSONValue(color);
		jcolors[L"gray"] = new JSONValue(gray);
		jcolors[L"hsv"] = new JSONValue(hsv);
		root[L"colors"] = new JSONValue(jcolors);
	}

	root[L"dataType"] = new JSONValue(L"YML");
	wstringstream ws;
	ws << data.c_str();
	root[L"data"] = new JSONValue(ws.str());

	return new wstring(JSONValue(root).Stringify());
}

string* CommunicationFormatterJSON::formatRequest(const char * data) {

	JSONValue* req = JSON::Parse(data);
	std::string* request = new string();
	if (req->HasChild(L"action")) {
		std::wstring waction = req->Child(L"action")->AsString();
		std::string saction(waction.begin(), waction.end());
		saction.append(" ");
		request->append("-");
		request->append(saction);
	}
	if (req->HasChild(Constants::WPARAM_IDXS.c_str())) {
		request->append("-" + Constants::PARAM_IDXS + " ");
		JSONArray indexes =
				req->Child(Constants::WPARAM_IDXS.c_str())->AsArray();
		for (unsigned int i = 0; i < indexes.size(); i++) {
			JSONValue* v = indexes.at(i);
			std::string sv = std::to_string((int) v->AsNumber());
			sv.append(" ");
			request->append(sv);
		}
	}
	if (req->HasChild(Constants::WPARAM_SCENEID.c_str())) {
		request->append("-" + Constants::PARAM_SCENEID + " ");
		std::string scenario =
				std::to_string(
						(int) req->Child(Constants::WPARAM_SCENEID.c_str())->AsNumber());
		scenario.append(" ");
		request->append(scenario);
	}
	if (req->HasChild(Constants::WOPTIONAL_FLAGS.c_str())) {
		JSONObject optionalFlags = req->Child(
				Constants::WOPTIONAL_FLAGS.c_str())->AsObject();
		if (optionalFlags.find(Constants::WPARAM_MIN_RATIO.c_str())
				!= optionalFlags.end()) {
			request->append("-" + Constants::PARAM_MIN_RATIO + " ");
			std::string mr =
					std::to_string(
							(float) optionalFlags.find(
									Constants::WPARAM_MIN_RATIO.c_str())->second->AsNumber());
			mr.append(" ");
			request->append(mr);
		}
		if (optionalFlags.find(Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())
				!= optionalFlags.end()) {
			request->append("-" + Constants::PARAM_MIN_MATCHES_ALLOWED + " ");
			std::string mma =
					std::to_string(
							(int) optionalFlags.find(
									Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())->second->AsNumber());
			mma.append(" ");
			request->append(mma);
		}
	}
	return request;
}

CommunicationFormatterJSON::~CommunicationFormatterJSON() {
}


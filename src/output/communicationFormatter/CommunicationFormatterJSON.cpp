/*
 * CommunicationFormatterJSON.cpp
 *
 *  Created on: May 23, 2014
 *      Author: renx
 */

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <utils/Constants.hpp>

using namespace std;
using namespace cv;
using namespace Anakin;

CommunicationFormatterJSON::CommunicationFormatterJSON() {
}

wstring CommunicationFormatterJSON::outputResponse(string requestID,
		e_category category, vector<wstring *> values) {

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
	switch (category){
		case CF_PATTERN_MATCHING: {
			ys << "PATTERN";
			break;
		}
		case CF_CACHE_IDX_ADD: {
			ys << "INDEX ADD";
			break;
		}
		case CF_CACHE_IDX_DEL: {
			ys << "INDEX DELETE";
			break;
		}
		case CF_CACHE_IDX_UPD: {
			ys << "INDEX UPDATE";
			break;
		}
		case CF_CACHE_IDX_STATUS: {
			ys << "INDEX STATUS";
			break;
		}
	}
	root[L"category"] = new JSONValue(ys.str());
	JSONArray valuesJSON;
	for (uint v = 0; v < values.size(); v++) {
		wstring* auxiliar = values.at(v);
		JSONValue *auxValue = new JSONValue(*auxiliar);
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	JSONValue *returnValue = new JSONValue(root);
	return returnValue->Stringify().c_str();

}

wstring CommunicationFormatterJSON::outputError(e_error errorType,
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
	JSONValue *value = new JSONValue(root);
	return value->Stringify().c_str();
}

wstring CommunicationFormatterJSON::format(const char * data) {
	return (JSON::Parse(data))->Stringify().c_str();
}

wstring CommunicationFormatterJSON::format(char mode, string data, char colors){
	/*  Result as JSONObject

	 root    -> type ("pattern" | "histogram" | "landscape")
	 -> colors (only if type != "pattern")   ->  color (bool)
	 ->  gray (bool)
	 ->  hsv (bool)
	 -> dataType ("YML" | "XML")
	 -> data (string)
	 */

	JSONObject root;

	if (mode & I_CommunicationFormatter::CF_PATTERNS) {
		root[L"type"] = new JSONValue(L"pattern");
	} else if (mode & I_CommunicationFormatter::CF_HISTOGRAMS) {
		root[L"type"] = new JSONValue(L"histogram");
	} else if (mode & I_CommunicationFormatter::CF_LANDSCAPE) {
		root[L"type"] = new JSONValue(L"landscape");
	}

	if ((mode & I_CommunicationFormatter::CF_HISTOGRAMS) || (mode & I_CommunicationFormatter::CF_LANDSCAPE)) {
		JSONObject jcolors;
		bool color = colors & I_CommunicationFormatter::CF_COLOR;
		bool gray = colors & I_CommunicationFormatter::CF_GRAY;
		bool hsv = colors & I_CommunicationFormatter::CF_HSV;
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

string CommunicationFormatterJSON::formatRequest(const char * data){
	JSONValue* req = JSON::Parse(data);
	std::string request = "";
		if (req->HasChild(L"action")) {
			std::wstring waction = req->Child(L"action")->AsString();
			std::string saction(waction.begin(), waction.end());
			request += "-" + saction + " ";
		}
		if (req->HasChild(Constants::WPARAM_IDXS.c_str())) {
			request += "-" + Constants::PARAM_IDXS + " ";
			JSONArray indexes =
					req->Child(Constants::WPARAM_IDXS.c_str())->AsArray();
			for (int i = 0; i < indexes.size(); i++) {
				JSONValue* v = indexes.at(i);
				std::string sv = std::to_string((int) v->AsNumber());
				request += sv + " ";
			}
		}
		if (req->HasChild(Constants::WPARAM_SCENEID.c_str())) {
			request += "-" + Constants::PARAM_SCENEID + " ";
			std::string scenario =
					std::to_string(
							(int) req->Child(Constants::WPARAM_SCENEID.c_str())->AsNumber());
			request += scenario + " ";
		}
		if (req->HasChild(Constants::WOPTIONAL_FLAGS.c_str())) {
			JSONObject optionalFlags = req->Child(
					Constants::WOPTIONAL_FLAGS.c_str())->AsObject();
			if (optionalFlags.find(Constants::WPARAM_MIN_RATIO.c_str())
					!= optionalFlags.end()) {
				request += "-" + Constants::PARAM_MIN_RATIO + " ";
				std::string mr =
						std::to_string(
								(float) optionalFlags.find(
										Constants::WPARAM_MIN_RATIO.c_str())->second->AsNumber());
				request += mr + " ";
			}
			if (optionalFlags.find(Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())
					!= optionalFlags.end()) {
				request += "-" + Constants::PARAM_MIN_MATCHES_ALLOWED + " ";
				std::string mma =
						std::to_string(
								(int) optionalFlags.find(
										Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())->second->AsNumber());
				request += mma + " ";
			}
		}
		return request;
}

CommunicationFormatterJSON::~CommunicationFormatterJSON() {
}


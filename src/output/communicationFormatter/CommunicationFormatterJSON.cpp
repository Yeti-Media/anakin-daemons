// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:t -*-
/*
 * CommunicationFormatterJSON.cpp
 *
 *  Created on: May 23, 2014
 *      Author: Renzo Bianchini
 */

#include <output/communicationFormatter/CommunicationFormatterJSON.hpp>
#include <utils/Constants.hpp>
#include <string>
#include <boost/locale.hpp>

using namespace std;
using namespace cv;
using namespace Anakin;

CommunicationFormatterJSON::CommunicationFormatterJSON() {
}

Ptr<wstring> CommunicationFormatterJSON::outputResponse(
		const string & requestID, e_category category,
		const Ptr<vector<Ptr<wstring>>> & values) {

	/*  Result as wstring representing a JSONObject

	 root    -> requestID (string)

	 -> category (string)

	 -> values (JSONArray)
	 */

	JSONObject root;
	root[L"requestID"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(requestID));
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
	case CF_DETECT_FACE: {
		root[L"category"] = new JSONValue(L"detect_face");
		break;
	}
	default: {
		root[L"category"] = new JSONValue(L"NONE---ERROR");
		break;
	}
	}
	JSONArray valuesJSON;
	for (uint v = 0; v < values->size(); v++) {
		JSONValue *auxValue = JSON::Parse(values->at(v)->c_str());
		valuesJSON.push_back(auxValue);
	}

	root[L"values"] = new JSONValue(valuesJSON);

	return makePtr<wstring>(JSONValue(root).Stringify());

}

Ptr<wstring> CommunicationFormatterJSON::outputError(e_error errorType,
		const string & message, const string & origin) {

	/*  Result as wstring representing a JSONObject

	 root    -> error_type (string)

	 -> message (string)

	 -> origin (string)
	 */

	JSONObject root;
	wstring werror_type;
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
	root[L"message"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(message));
	root[L"origin"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(origin));
	return makePtr<wstring>(JSONValue(root).Stringify());
}

Ptr<wstring> CommunicationFormatterJSON::format(const char * data) {

	return makePtr<wstring>((JSON::Parse(data))->Stringify());
}

Ptr<wstring> CommunicationFormatterJSON::format(e_mode mode,
		const string & data, e_color colors) {
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
	root[L"data"] = new JSONValue(
			boost::locale::conv::utf_to_utf<wchar_t>(data));

	return makePtr<wstring>(JSONValue(root).Stringify());
}

Ptr<wstring> CommunicationFormatterJSON::format(
		const Ptr<vector<string>> & text) {

	JSONObject root;
	JSONArray texts;
	for (uint v = 0; v < text->size(); v++) {
		JSONObject jText;
		jText[L"text"] = new JSONValue(
				boost::locale::conv::utf_to_utf<wchar_t>(text->at(v)));
		texts.push_back(new JSONValue(jText));
	}

	root[L"values"] = new JSONValue(texts);

	return makePtr<wstring>(JSONValue(root).Stringify());
}

Ptr<string> CommunicationFormatterJSON::formatRequest(const char * data) {

	JSONValue* req = JSON::Parse(data);
	Ptr<string> request = makePtr<string>();
	if (!req) return request;
	if (req->HasChild(L"action")) {
		wstring waction = req->Child(L"action")->AsString();
		string saction(waction.begin(), waction.end());
		saction.append(" ");
		request->append("-");
		request->append(saction);
	}
	if (req->HasChild(L"ocr")) {
		wstring waction = req->Child(L"ocr")->AsString();
		string saction(waction.begin(), waction.end());
		request->append("\"" + saction + "\"");
	}
	if (req->HasChild(L"img")) {
		wstring waction = req->Child(L"img")->AsString();
		string saction(waction.begin(), waction.end());
		request->append("\"" + saction + "\"");
	}
	if (req->HasChild(Constants::WPARAM_IDXS.c_str())) {
		request->append("-" + Constants::PARAM_IDXS + " ");
		JSONArray indexes =
				req->Child(Constants::WPARAM_IDXS.c_str())->AsArray();
		for (unsigned int i = 0; i < indexes.size(); i++) {
			JSONValue* v = indexes.at(i);
			string sv = to_string((int) v->AsNumber());
			sv.append(" ");
			request->append(sv);
		}
	}
	if (req->HasChild(Constants::WPARAM_SCENEID.c_str())) {
		request->append("-" + Constants::PARAM_SCENEID + " ");
		string scenario =
				to_string(
						(int) req->Child(Constants::WPARAM_SCENEID.c_str())->AsNumber());
		scenario.append(" ");
		request->append(scenario);
	}
	if (req->HasChild(L"original_width")) {
		request->append(" -original_width ");
		string sv = to_string((int) req->Child(L"original_width")->AsNumber());
		request->append(sv);
	}
	if (req->HasChild(L"original_height")) {
		request->append(" -original_height ");
		string sv = to_string((int) req->Child(L"original_height")->AsNumber());
		request->append(sv);
	}
	if (req->HasChild(Constants::WOPTIONAL_FLAGS.c_str())) {
		JSONObject optionalFlags = req->Child(
				Constants::WOPTIONAL_FLAGS.c_str())->AsObject();
		if (optionalFlags.find(Constants::WPARAM_MIN_RATIO.c_str())
				!= optionalFlags.end()) {
			request->append("-" + Constants::PARAM_MIN_RATIO + " ");
			string mr =
					to_string(
							(float) optionalFlags.find(
									Constants::WPARAM_MIN_RATIO.c_str())->second->AsNumber());
			mr.append(" ");
			request->append(mr);
		}
		if (optionalFlags.find(Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())
				!= optionalFlags.end()) {
			request->append("-" + Constants::PARAM_MIN_MATCHES_ALLOWED + " ");
			string mma =
					to_string(
							(int) optionalFlags.find(
									Constants::WPARAM_MIN_MATCHES_ALLOWED.c_str())->second->AsNumber());
			mma.append(" ");
			request->append(mma);
		}
	}
	delete req;
	return request;
}

CommunicationFormatterJSON::~CommunicationFormatterJSON() {
}


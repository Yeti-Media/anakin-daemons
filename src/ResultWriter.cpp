#include <ResultWriter.hpp>

using namespace Anakin;
using namespace std;

wstring ResultWriter::output(char mode, string data, char colors) {
    return resultAsJSONValue(mode, data, colors)->Stringify().c_str();
}

JSONValue* ResultWriter::resultAsJSONValue(char mode, string data, char colors) {
    /*  Result as JSONObject

        root    -> type ("pattern" | "histogram" | "landscape")
                -> colors (only if type != "pattern")   ->  color (bool)
                                                        ->  gray (bool)
                                                        ->  hsv (bool)
                -> dataType ("YML" | "XML")
                -> data (string)
    */
    JSONObject root;

    if (mode & RW_PATTERNS) {
        root[L"type"] = new JSONValue(L"pattern");
    } else if (mode & RW_HISTOGRAMS) {
        root[L"type"] = new JSONValue(L"histogram");
    } else if (mode & RW_LANDSCAPE) {
        root[L"type"] = new JSONValue(L"landscape");
    }

    if (mode & RW_HISTOGRAMS || mode & RW_LANDSCAPE) {
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
	return value;
}

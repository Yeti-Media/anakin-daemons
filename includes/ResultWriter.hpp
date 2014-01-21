#ifndef RESULTWRITER_HPP_INCLUDED
#define RESULTWRITER_HPP_INCLUDED

#include <JSON.h>
#include <JSONValue.h>
#include <string>
#include <sstream>

namespace Anakin {

class ResultWriter {


    public:
        static std::wstring output(char mode, std::string data, char colors=0);
        static JSONValue* resultAsJSONValue(char mode, std::string data, char colors=0);
        static const char RW_PATTERNS = 1;
        static const char RW_HISTOGRAMS = 2;
        static const char RW_LANDSCAPE = 4;

        static const char RW_COLOR = 8;
        static const char RW_GRAY = 16;
        static const char RW_HSV = 32;

};

};

#endif // RESULTWRITER_HPP_INCLUDED

#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <string>

namespace Anakin {

class Constants {
    public:
        static const char COLOR = 1;
        static const char GRAY = 2;
        static const char HSV = 4;
        static const char LANDSCAPE = 8;
        static const char HISTOGRAM = 16;
        static const char PATTERN = 32;
        static const char INDEX = 64;
        static const char SCENE = 128;

        static const std::string INSERT_COMMAND;
        static const std::string SELECT_ALL_COMMAND;
        static const std::string SELECT_COMMAND;

        static const std::string USER_TABLE;
        static const std::string USER_TABLE_ID;

        static const std::string PATTERN_TABLE;
        static const std::string PATTERN_TABLE_ID;
        static const std::string PATTERN_TABLE_LABEL;
        static const std::string PATTERN_TABLE_DATA;

        static const std::string HISTOGRAM_TABLE;
        static const std::string LANDSCAPE_TABLE;

        static const std::string HISTLAND_TABLE_ID;
        static const std::string HISTLAND_TABLE_LABEL;
        static const std::string HISTLAND_TABLE_CDATA;
        static const std::string HISTLAND_TABLE_GDATA;
        static const std::string HISTLAND_TABLE_HDATA;

        static const std::string RELATION_TABLE_USER_ID;
        static const std::string RELATION_TABLE_PATTERN_ID;
        static const std::string RELATION_TABLE_HISTOGRAM_ID;
        static const std::string RELATION_TABLE_LANDSCAPE_ID;
        static const std::string RELATION_TABLE_SMATCHER_ID;

        static const std::string USER_PATTERNS_TABLE;
        static const std::string USER_HISTOGRAMS_TABLE;
        static const std::string USER_LANDSCAPES_TABLE;
        static const std::string USER_SMATCHERS_TABLE;

        static const std::string SMATCHER_TABLE;
        static const std::string SMATCHER_TABLE_ID;
        static const std::string SMATCHER_TABLE_FBM_ID;
        static const std::string SMATCHER_TABLE_INDEX_ID;

        static const std::string SCENE_TABLE;
        static const std::string SCENE_TABLE_ID;
        static const std::string SCENE_TABLE_DESC;
        static const std::string SCENE_TABLE_KEYPTS;

        static const std::string SMATCHER_PATTERNS_TABLE;
        static const std::string SMATCHER_PATTERNS_TABLE_IID;
        static const std::string SMATCHER_PATTERNS_TABLE_PID;
        static const std::string SMATCHER_PATTERNS_TABLE_PIDX;


    protected:
    private:
};
};

#endif // CONSTANTS_HPP

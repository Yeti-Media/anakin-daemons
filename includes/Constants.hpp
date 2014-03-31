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
        static const std::string UPDATE_COMMAND;

        static const std::string USER_TABLE;
        static const std::string USER_TABLE_ID;

        static const std::string PATTERN_TABLE;
        static const std::string PATTERN_TABLE_ID;
        static const std::string PATTERN_TABLE_USER_ID;
        static const std::string PATTERN_TABLE_TRAINER_ID;
        static const std::string PATTERN_TABLE_CATEGORY_ID;
        static const std::string PATTERN_TABLE_POSITION;

        static const std::string HISTLAND_TABLE;

        static const std::string HISTLAND_TABLE_ID;
        static const std::string HISTLAND_TABLE_CDATA;
        static const std::string HISTLAND_TABLE_GDATA;
        static const std::string HISTLAND_TABLE_HDATA;
        static const std::string HISTLAND_TABLE_PATTERN_ID;

        static const std::string TRAINER_TABLE;
        static const std::string TRAINER_TABLE_ID;
        static const std::string TRAINER_TABLE_XML_ID;
        static const std::string TRAINER_TABLE_IF_ID;
        static const std::string TRAINER_TABLE_USER_ID;

        static const std::string SCENE_TABLE;
        static const std::string SCENE_TABLE_ID;
        static const std::string SCENE_TABLE_DATA;

        static const std::string DESCRIPTORS_TABLE;
        static const std::string DESCRIPTORS_TABLE_ID;
        static const std::string DESCRIPTORS_TABLE_DATA;
        static const std::string DESCRIPTORS_TABLE_PATTERN_ID;

        static const std::string CATEGORIES_TABLE;
        static const std::string CATEGORIES_TABLE_ID;
        static const std::string CATEGORIES_TABLE_NAME;

        static const std::string CATEGORIES_MATCHING;
        static const std::string CATEGORIES_COMPARISON;
        static const std::string CATEGORIES_LANDSCAPES;


        //REQUESTS
        static const std::string ACTION_MATCH;
        static const std::string ACTION_ADDIDX;
        static const std::string ACTION_DELIDX;
        static const std::string ACTION_UPDIDX;
        static const std::string ACTION_STATUSIDX;

        static const std::string PARAM_IDXS;
        static const std::string PARAM_SCENEID;
        static const std::string PARAM_REQID;

        static const std::wstring WACTION_MATCH;
        static const std::wstring WACTION_ADDIDX;
        static const std::wstring WACTION_DELIDX;
        static const std::wstring WACTION_UPDIDX;
        static const std::wstring WACTION_STATUSIDX;

        static const std::wstring WPARAM_IDXS;
        static const std::wstring WPARAM_SCENEID;
        static const std::wstring WPARAM_REQID;

    protected:
    private:
};
};

#endif // CONSTANTS_HPP

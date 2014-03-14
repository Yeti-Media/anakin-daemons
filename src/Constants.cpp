#include "Constants.hpp"

using namespace Anakin;

const std::string Constants::INSERT_COMMAND = "INSERT INTO ";
const std::string Constants::SELECT_ALL_COMMAND = "SELECT * FROM ";
const std::string Constants::SELECT_COMMAND = "SELECT ";

const std::string Constants::USER_TABLE = "User";
const std::string Constants::USER_TABLE_ID = "id";

const std::string Constants::PATTERN_TABLE = "Pattern";
const std::string Constants::PATTERN_TABLE_ID = "id";
const std::string Constants::PATTERN_TABLE_LABEL = "label";
const std::string Constants::PATTERN_TABLE_DATA = "data";

const std::string Constants::HISTOGRAM_TABLE = "Histogram";
const std::string Constants::LANDSCAPE_TABLE = "Landscape";

const std::string Constants::HISTLAND_TABLE_ID = "id";
const std::string Constants::HISTLAND_TABLE_LABEL = "label";
const std::string Constants::HISTLAND_TABLE_CDATA = "color_data";
const std::string Constants::HISTLAND_TABLE_GDATA = "gray_data";
const std::string Constants::HISTLAND_TABLE_HDATA = "hsv_data";

const std::string Constants::RELATION_TABLE_USER_ID = "user_id";
const std::string Constants::RELATION_TABLE_PATTERN_ID = "pattern_id";
const std::string Constants::RELATION_TABLE_HISTOGRAM_ID = "histogram_id";
const std::string Constants::RELATION_TABLE_LANDSCAPE_ID = "landscape_id";
const std::string Constants::RELATION_TABLE_SMATCHER_ID = "smatcher_id";

const std::string Constants::USER_PATTERNS_TABLE = "User_Patterns";
const std::string Constants::USER_HISTOGRAMS_TABLE = "User_Histograms";
const std::string Constants::USER_LANDSCAPES_TABLE = "User_Landscapes";
const std::string Constants::USER_SMATCHERS_TABLE = "User_SerializedMatchers";

const std::string Constants::SMATCHER_TABLE = "SerializedMatcher";
const std::string Constants::SMATCHER_TABLE_ID = "id";
const std::string Constants::SMATCHER_TABLE_FBM_ID = "fbm_id";
const std::string Constants::SMATCHER_TABLE_INDEX_ID = "index_id";

const std::string Constants::SCENE_TABLE = "Scene";
const std::string Constants::SCENE_TABLE_ID = "id";
const std::string Constants::SCENE_TABLE_DESC = "descriptors";
const std::string Constants::SCENE_TABLE_KEYPTS = "keypts";

const std::string Constants::SMATCHER_PATTERNS_TABLE = "Smatcher_Patterns";
const std::string Constants::SMATCHER_PATTERNS_TABLE_IID = "indexID";
const std::string Constants::SMATCHER_PATTERNS_TABLE_PID = "patternID";
const std::string Constants::SMATCHER_PATTERNS_TABLE_PIDX = "patternIDX";

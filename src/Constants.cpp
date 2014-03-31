#include "Constants.hpp"

using namespace Anakin;

const std::string Constants::INSERT_COMMAND = "INSERT INTO ";
const std::string Constants::SELECT_ALL_COMMAND = "SELECT * FROM ";
const std::string Constants::SELECT_COMMAND = "SELECT ";
const std::string Constants::UPDATE_COMMAND = "UPDATE ";

const std::string Constants::USER_TABLE = "users";
const std::string Constants::USER_TABLE_ID = "id";

const std::string Constants::PATTERN_TABLE = "patterns";
const std::string Constants::PATTERN_TABLE_ID = "id";
const std::string Constants::PATTERN_TABLE_USER_ID = "user_id";
const std::string Constants::PATTERN_TABLE_TRAINER_ID = "trainer_id";
const std::string Constants::PATTERN_TABLE_CATEGORY_ID = "category_id";
const std::string Constants::PATTERN_TABLE_POSITION = "position";

const std::string Constants::HISTLAND_TABLE = "histrograms";

const std::string Constants::HISTLAND_TABLE_ID = "id";
const std::string Constants::HISTLAND_TABLE_CDATA = "color";
const std::string Constants::HISTLAND_TABLE_GDATA = "gray";
const std::string Constants::HISTLAND_TABLE_HDATA = "hsv";
const std::string Constants::HISTLAND_TABLE_PATTERN_ID = "pattern_id";

const std::string Constants::TRAINER_TABLE = "trainers";
const std::string Constants::TRAINER_TABLE_ID = "id";
const std::string Constants::TRAINER_TABLE_XML_ID = "xml_file";
const std::string Constants::TRAINER_TABLE_IF_ID = "if_file";
const std::string Constants::TRAINER_TABLE_USER_ID = "user_id";

const std::string Constants::SCENE_TABLE = "scenarios";
const std::string Constants::SCENE_TABLE_ID = "id";
const std::string Constants::SCENE_TABLE_DATA = "descriptors";

const std::string Constants::DESCRIPTORS_TABLE = "descriptors";
const std::string Constants::DESCRIPTORS_TABLE_ID = "id";
const std::string Constants::DESCRIPTORS_TABLE_DATA = "body";
const std::string Constants::DESCRIPTORS_TABLE_PATTERN_ID = "pattern_id";

const std::string Constants::CATEGORIES_TABLE = "categories";
const std::string Constants::CATEGORIES_TABLE_ID = "id";
const std::string Constants::CATEGORIES_TABLE_NAME = "title";

const std::string Constants::CATEGORIES_MATCHING = "matching";
const std::string Constants::CATEGORIES_COMPARISON = "comparison";
const std::string Constants::CATEGORIES_LANDSCAPES = "landscapes";

//REQUESTS
const std::string Constants::ACTION_MATCH = "matching";
const std::string Constants::ACTION_ADDIDX = "add_indexes";
const std::string Constants::ACTION_DELIDX = "remove_indexes";
const std::string Constants::ACTION_UPDIDX = "update_indexes";
const std::string Constants::ACTION_STATUSIDX = "status_indexes";

const std::string Constants::PARAM_IDXS = "indexes";
const std::string Constants::PARAM_SCENEID = "scenario";
const std::string Constants::PARAM_REQID = "reqID";

const std::wstring Constants::WACTION_MATCH = L"matching";
const std::wstring Constants::WACTION_ADDIDX = L"add_indexes";
const std::wstring Constants::WACTION_DELIDX = L"remove_indexes";
const std::wstring Constants::WACTION_UPDIDX = L"update_indexes";
const std::wstring Constants::WACTION_STATUSIDX = L"status_indexes";

const std::wstring Constants::WPARAM_IDXS = L"indexes";
const std::wstring Constants::WPARAM_SCENEID = L"scenario";
const std::wstring Constants::WPARAM_REQID = L"reqID";

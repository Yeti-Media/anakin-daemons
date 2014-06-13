/*
 * HelpPatternMatcher.cpp
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#include <utils/help/HelpPatternMatcher.hpp>

namespace Anakin {

bool HelpPatternMatcher::showDaemonHelp() {
	return true;
}

HelpPatternMatcher::HelpPatternMatcher() {
	intro = "Starts N threads and process requests to add, delete or update "
			"trainers to the cache, and to process pattern matching requests. "
			"This application only uses data from the db.\n";

	usage =
			"-cacheStatus -reqID <ID>		: will return trainers cache status (trainers loaded and free cache space)\n"
					"-addIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will add trainers with ID = I1, I2, ... In to the cache\n"
					"-remIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will remove trainers with ID = I1, I2, ... In from the cache\n"
					"-updateIndexes -indexes <I1 I2 ... In> -reqID <ID>		: will reload trainers with ID = I1, I2, ... In to the cache\n"
					"-pmatch -sceneID <SID> -indexes <I1 I2 ... In> -reqID <ID>	: will load scene with id SID and search for patterns	using trainers with ID = I1, I2, ... In\n";

	flags = "-reqID <ID>			: this is just to set an ID to every request\n"
			"-indexes <I1 I2 ... In>		: set a range of indexes\n"
			"-sceneID <SID>			: scene ID to match\n";

	examples =
			"-addIndexes -indexes 2 3 5 7 -reqID r001	(1)\n"
					"-remIndexes -indexes 7 -reqID r002		(2)\n"
					"-pmatch -sceneID 1 -indexes 3 -reqID r003	(3)\n\n"
					"In (1) the trainers 2, 3, 5 and 7 are loaded into the cache. After (2) the trainer with ID 7 is no longer in the cache.\n"
					"(3) will search for pattern in the scene with ID 1 using trainer with ID 3\n\n"

					"Results (JSON) :\n\n"

					"root   -> requestID (string)\n"
					"       -> category (string)\n"
					"       -> values (JSONArray)\n\n"

					"values (add, removed) :\n"
					"root    -> index_added (long) (this value is -1 for remIndexes requests)\n"
					"        -> index_removed (long) (this value is -1 for addIndexes requests that didn't remove any trainer)\n"
					"        -> cache_free_space (long)\n\n"

					"values (update) :\n"
					"root    -> index_updated (long)\n"
					"values (status) :\n"
					"root    -> cache_free_space (long)\n"
					"        -> indexes (JSONArray)    -> index (long)\n\n"

					"values (pmatch) :\n"
					"root    -> scene label (string)\n"
					"        -> values (JSONArray)   -> center   -> x (float)\n"
					"                                            -> y (float)\n"
					"                                -> pattern label (string)\n"
					"                                -> keypoints (JSONArray) -> pos  -> x (float)\n"
					"                                                                 -> y (float)\n"
					"                                                         -> angle (float)\n"
					"                                                         -> size (float)\n"
					"                                                         -> response (float)\n\n"
					"Note: currently keypoints are disabled in the JSON output\n";

}

HelpPatternMatcher::~HelpPatternMatcher() {
	// TODO Auto-generated destructor stub
}

} /* namespace Anakin */

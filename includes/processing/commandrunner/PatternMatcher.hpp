/*
 * PatternMatcher.hpp
 *
 *  Created on: 20/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef PATTERNMATCHER_HPP_
#define PATTERNMATCHER_HPP_

#include <processing/commandrunner/CommandRunner.hpp>
#include <string>
#include <vector>
#include <utils/help/Help.hpp>
#include <processing/Flags.hpp>
#include <utils/files/QuickLZ.hpp>
#include <output/communicationFormatter/ICommunicationFormatterMatching.hpp>

namespace Anakin {

enum E_PatternMatchingAction {
	NONE = 0, MATCH = 1, ADDIDXS = 2, DELIDXS = 3, UPDIDXS = 4, IDXSTATUS = 5
};

class PatternMatcher: public CommandRunner {
public:
	PatternMatcher();
	virtual ~PatternMatcher();

	void validateRequest(const Ptr<vector<string>> & input);

	void initializeCommandRunner(const Ptr<DataOutput> & out,
			const Ptr<SFBMCache> & cache);

	void run();

	Help* getHelp();

protected:
	E_PatternMatchingAction action = NONE;
	I_CommunicationFormatterMatching* cfm;
	QuickLZ* quickLZstate;
};

} /* namespace Anakin */

#endif /* PATTERNMATCHER_HPP_ */

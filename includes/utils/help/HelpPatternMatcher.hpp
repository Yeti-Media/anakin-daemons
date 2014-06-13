/*
 * HelpPatternMatching.hpp
 *
 *  Created on: 23/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef HELPPATTERNMATCHER_HPP_
#define HELPPATTERNMATCHER_HPP_

#include <utils/help/Help.hpp>

namespace Anakin {

/**
 * PatternMatching Help documentation
 */
class HelpPatternMatcher: public Help {
public:
	HelpPatternMatcher();
	virtual ~HelpPatternMatcher();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif /* HELPPATTERNMATCHING_HPP_ */

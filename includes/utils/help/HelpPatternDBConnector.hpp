/*
 * HelpDBConnector.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELPPATTERNDBCONNECTOR_HPP
#define HELPPATTERNDBCONNECTOR_HPP

#include <utils/help/Help.hpp>

namespace Anakin {

/**
 * DbConnector Help documentation
 */
class HelpPatternDBConnector: public Help {
public:
	HelpPatternDBConnector();
	~HelpPatternDBConnector();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif // HELPDBCONNECTOR_HPP

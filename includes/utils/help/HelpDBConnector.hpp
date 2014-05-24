/*
 * HelpDBConnector.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELPDBCONNECTOR_HPP
#define HELPDBCONNECTOR_HPP

#include <utils/help/Help.hpp>

namespace Anakin {

/**
 * Implement the usage documentation for DBConnector
 */
class HelpDBConnector: public Help {
public:
	HelpDBConnector();
	~HelpDBConnector();
};

} /* namespace Anakin */


#endif // HELPDBCONNECTOR_HPP

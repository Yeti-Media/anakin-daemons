/*
 * HelpOCRDemo.hpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef HELPOCRDEMO_HPP_
#define HELPOCRDEMO_HPP_

#include <utils/help/Help.hpp>

namespace Anakin {

class HelpOCRDemo: public Help {
public:
	HelpOCRDemo();
	virtual ~HelpOCRDemo();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif /* HELPOCRDEMO_HPP_ */

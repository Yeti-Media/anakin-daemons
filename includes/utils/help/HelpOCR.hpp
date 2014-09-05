/*
 * HelpOCR.hpp
 *
 *  Created on: 18/8/2014
 *      Author: Franco Pellegrini
 */

#ifndef HELPOCR_HPP_
#define HELPOCR_HPP_

#include <utils/help/Help.hpp>

namespace Anakin {

class HelpOCR: public Help {
public:
	HelpOCR();
	virtual ~HelpOCR();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif /* HELPOCR_HPP_ */

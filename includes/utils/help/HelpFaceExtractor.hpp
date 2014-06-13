/*
 * HelpFaceExtractor.hpp
 *
 *  Created on: 13/06/2014
 *      Author: Franco Pellegrini
 */

#ifndef HELPFACEEXTRACTOR_HPP_
#define HELPFACEEXTRACTOR_HPP_

#include <utils/help/Help.hpp>

namespace Anakin {

class HelpFaceExtractor: public Help {
public:
	HelpFaceExtractor();
	virtual ~HelpFaceExtractor();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif /* HELPFACEEXTRACTOR_HPP_ */

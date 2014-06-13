/*
 * HelpTrainer.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELPPATTERNTRAINER_H_
#define HELPPATTERNTRAINER_H_

#include <utils/help/Help.hpp>

namespace Anakin {

/**
 * Trainer Help documentation
 */
class HelpPatternTrainer: public Help {
public:
	HelpPatternTrainer();
	virtual ~HelpPatternTrainer();

	bool showDaemonHelp();
};

} /* namespace Anakin */

#endif /* HELPTRAINER_H_ */

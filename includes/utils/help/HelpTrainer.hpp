/*
 * HelpTrainer.hpp
 *
 *  Created on: 23/05/2014
 *      Author: franco
 */

#ifndef HELPTRAINER_H_
#define HELPTRAINER_H_

#include <utils/help/Help.hpp>

namespace Anakin {

/**
 * Implement the usage documentation for Trainer
 */
class HelpTrainer: public Help {
public:
	HelpTrainer();
	virtual ~HelpTrainer();
};

} /* namespace Anakin */

#endif /* HELPTRAINER_H_ */

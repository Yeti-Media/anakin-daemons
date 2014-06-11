/*
 * DataOutputWorkerConsole.hpp
 *
 *  Created on: 03/06/2014
 *      Author: franco
 */

#ifndef DATAOUTPUTWORKERCONSOLE_HPP_
#define DATAOUTPUTWORKERCONSOLE_HPP_

#include <output/Msj.hpp>
#include <output/workers/DataOutputWorker.hpp>
#include <utils/BlockingQueue.hpp>

namespace Anakin {

class DataOutputWorkerConsole: public DataOutputWorker {
public:
	DataOutputWorkerConsole(BlockingQueue<Msj*>* workingQueue);
	virtual ~DataOutputWorkerConsole();

	void processOutput(Msj* msj);
};

} /* namespace Anakin */

#endif /* DATAOUTPUTWORKERCONSOLE_HPP_ */

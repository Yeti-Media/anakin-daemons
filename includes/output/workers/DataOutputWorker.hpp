/*
 * DataOutputWorker.hpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef DATAOUTPUTWORKER_HPP_
#define DATAOUTPUTWORKER_HPP_

#include <output/Msj.hpp>
#include <utils/BlockingQueue.hpp>

using namespace std;
namespace Anakin {

/**
 * This class is executed in another thread, and will dispatch the data stored
 * in the concurrent queue.
 * processOutput must be implemented in a concrete class.
 */
class DataOutputWorker {
public:
	/**
	 * workingQueue: blocking queue to use with other threads to deliver data
	 */
	DataOutputWorker(BlockingQueue<Msj*>* workingQueue);
	virtual ~DataOutputWorker();

	/**
	 * process the msj and "write" the output
	 */
	virtual void processOutput(Msj* msj)=0;

	/**
	 * starts the worker listening cicle
	 */
	void start();
private:
	BlockingQueue<Msj*>* msjQueue;
};

} /* namespace Anakin */

#endif /* DATAOUTPUTWORKER_HPP_ */

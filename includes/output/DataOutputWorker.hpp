/*
 * DataOutputWorker.hpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef DATAOUTPUTWORKER_HPP_
#define DATAOUTPUTWORKER_HPP_

#include <connection/HTTPSocket.hpp>
#include <output/DataEnumerates.hpp>
#include <output/Msj.hpp>
#include <utils/BlockingQueue.hpp>

using namespace std;
namespace Anakin {

/**
 * This class is executed in another thread, and will dispatch the data stored
 * in the concurrent queue.
 */
class DataOutputWorker {
public:
	/**
	 * outputType: specific implementation to choose for an output
	 * httpSocket: http socket to use, or NULL.
	 * workingQueue: blocking queue to use with other threads to deliver data
	 */
	DataOutputWorker(E_DataOutputType outputType, HTTPSocket* httpSocket,
			BlockingQueue<Msj*>* workingQueue);
	virtual ~DataOutputWorker();

	/**
	 * starts the worker listening cicle
	 */
	void start();
private:
	E_DataOutputType outputType;
	HTTPSocket* httpSocket;
	BlockingQueue<Msj*>* msjQueue;
};

} /* namespace Anakin */

#endif /* DATAOUTPUTWORKER_HPP_ */

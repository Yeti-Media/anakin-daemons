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

class DataOutputWorker {
public:
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

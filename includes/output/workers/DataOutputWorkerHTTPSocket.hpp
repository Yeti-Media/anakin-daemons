/*
 * DataOutputWorkerHTTPSocket.hpp
 *
 *  Created on: 03/06/2014
 *      Author: franco
 */

#ifndef DATAOUTPUTWORKERHTTPSOCKET_HPP_
#define DATAOUTPUTWORKERHTTPSOCKET_HPP_

#include <output/Msj.hpp>
#include <output/workers/DataOutputWorker.hpp>
#include <utils/BlockingQueue.hpp>

namespace Anakin {

class DataOutputWorkerHTTPSocket: public DataOutputWorker {
public:
	DataOutputWorkerHTTPSocket(BlockingQueue<Msj*>* workingQueue, HTTPSocket* httpSocket);
	virtual ~DataOutputWorkerHTTPSocket();

	void processOutput(Msj* msj);

private:
	HTTPSocket* httpSocket;
};

}
/* namespace Anakin */

#endif /* DATAOUTPUTWORKERHTTPSOCKET_HPP_ */

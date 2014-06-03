/*
 * DataOutputWorkerHTTPSocket.cpp
 *
 *  Created on: 03/06/2014
 *      Author: franco
 */

#include <connection/HTTPSocket.hpp>
#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataEnumerates.hpp>
#include <output/workers/DataOutputWorkerHTTPSocket.hpp>
#include <string>

using namespace std;
namespace Anakin {

DataOutputWorkerHTTPSocket::DataOutputWorkerHTTPSocket(
		BlockingQueue<Msj*>* workingQueue, HTTPSocket* httpSocket) :
		DataOutputWorker(workingQueue), httpSocket(httpSocket) {

}

DataOutputWorkerHTTPSocket::~DataOutputWorkerHTTPSocket() {
	// TODO Auto-generated destructor stub
}

void DataOutputWorkerHTTPSocket::processOutput(Msj* msj) {
	switch (msj->type) {
	//error data
	case error: {
		this->httpSocket->respond(msj->data, false, -1);
		LOG_F("ERROR") << msj->data;
		break;
	}
		//common data
	case common: {
		this->httpSocket->respond(msj->data, true, msj->reqID);
		break;
	}
	} //case
}

} /* namespace Anakin */

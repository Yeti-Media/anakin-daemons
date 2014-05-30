/*
 * DataOutputWorker.cpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/DataOutputWorker.hpp>
#include <iostream>
#include <string>

namespace Anakin {

DataOutputWorker::DataOutputWorker(E_DataOutputType outputType,
		HTTPSocket* httpSocket, BlockingQueue<Msj*>* workingQueue) :
		outputType(outputType), httpSocket(httpSocket), msjQueue(
				workingQueue) {
}

DataOutputWorker::~DataOutputWorker() {
}

void DataOutputWorker::start() {
	bool run = true;
	while (run) {
		Msj* msj = this->msjQueue->pop();
		if (msj != NULL) {
			switch (outputType) {
			//should never be used
			case none: {
				break;
			}
			//console output code
			case console: {
				switch (msj->type) {
				//error data
				case error: {
					flush(cerr);
					cerr << msj->data << endl;
					break;
				}
				//common data
				case common: {
					flush(cout);
					cout << msj->data << endl;
					break;
				}
				} //case
				break;
			}
			//http socket output code
			case http: {
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
				break;
			}
			} //case

			delete msj;
		} else {
			run = false;
		}
	}
}

} /* namespace Anakin */

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
			case none: {
				break;
			}
			case console: {
				switch (msj->type) {
				case error: {
					flush(cerr);
					cerr << msj->data << endl;
					break;
				}
				case common: {
					flush(cout);
					cout << msj->data << endl;
					break;
				}
				} //case
				break;
			}
			case http: {
				switch (msj->type) {
				case error: {
					this->httpSocket->respond(msj->data, false, -1);
					LOG_F("ERROR") << msj->data;
					break;
				}
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

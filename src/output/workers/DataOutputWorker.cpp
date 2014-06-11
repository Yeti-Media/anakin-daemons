/*
 * DataOutputWorker.cpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#include <logging/Log.hpp>
#include <logging/OutputPolicyFile.hpp>
#include <output/workers/DataOutputWorker.hpp>
#include <iostream>
#include <string>

using namespace std;

namespace Anakin {

DataOutputWorker::DataOutputWorker(BlockingQueue<Msj*>* workingQueue) :
		msjQueue(workingQueue) {
}

DataOutputWorker::~DataOutputWorker() {
}

void DataOutputWorker::start() {
	bool run = true;
	while (run) {
		Msj* msj = this->msjQueue->pop();
		if (msj != NULL) {
			processOutput(msj);
			delete msj;
		} else {
			run = false;
		}
	}
}

} /* namespace Anakin */

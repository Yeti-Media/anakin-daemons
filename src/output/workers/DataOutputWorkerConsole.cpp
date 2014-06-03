/*
 * DataOutputWorkerConsole.cpp
 *
 *  Created on: 03/06/2014
 *      Author: franco
 */

#include <output/workers/DataOutputWorkerConsole.hpp>
#include <iostream>
#include <string>

using namespace std;
namespace Anakin {

DataOutputWorkerConsole::DataOutputWorkerConsole(
		BlockingQueue<Msj*>* workingQueue) :
		DataOutputWorker(workingQueue) {
}

DataOutputWorkerConsole::~DataOutputWorkerConsole() {
	// TODO Auto-generated destructor stub
}

void DataOutputWorkerConsole::processOutput(Msj* msj) {
	switch (msj->type) {
	case error: { //error data
		flush(cerr);
		cerr << msj->data << endl;
		break;
	}
	case common: { //common data
		flush(cout);
		cout << msj->data << endl;
		break;
	}
	} //case
}

} /* namespace Anakin */

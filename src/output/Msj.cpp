/*
 * Msj.cpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#include <output/Msj.hpp>

namespace Anakin {

Msj::Msj(string* data, E_DataOutputMsjType msjType, int reqID) :
		data(data), type(msjType), reqID(reqID) {

}

Msj::Msj(string* data, E_DataOutputMsjType msjType) :
		data(data), type(msjType) {
	this->reqID = -1;
}

Msj::~Msj() {
	delete data;
}

} /* namespace Anakin */

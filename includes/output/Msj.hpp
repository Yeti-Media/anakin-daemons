/*
 * Msj.hpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef MSJ_HPP_
#define MSJ_HPP_

#include <output/DataEnumerates.hpp>
#include <string>

using namespace std;

namespace Anakin {

class Msj {
public:
	Msj(string data, E_DataOutputMsjType msjType, int reqID);
	virtual ~Msj();

	string data;
	E_DataOutputMsjType type;
	int reqID;
};

} /* namespace Anakin */

#endif /* MSJ_HPP_ */

/*
 * ClearVector.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: Renzo Bianchini
 */


#include <utils/ClearVector.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace Anakin;

void Anakin::deleteVector(vector<wstring*>* lalala){
	for(unsigned int i = 0; i < lalala->size(); i++){
		delete lalala->at(i);
	}
	lalala->clear();
}



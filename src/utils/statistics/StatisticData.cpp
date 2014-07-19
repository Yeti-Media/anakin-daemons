/*
 * StatisticData.cpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#include <utils/statistics/StatisticData.hpp>

namespace Anakin {

StatisticData::StatisticData(double time) :
		samples(1), totalTime(time), bestTime(time), worstTime(time) {
}

StatisticData::~StatisticData() {
	// TODO Auto-generated destructor stub
}

void StatisticData::addTime(double time) {
	samples++;
	totalTime += time;
	if (time < bestTime) {
		bestTime = time;
	}
	if (time > worstTime) {
		worstTime = time;
	}
}

double StatisticData::getAvgTime() {
	return totalTime / samples;
}

} /* namespace Anakin */

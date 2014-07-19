/*
 * StatisticsCollector.hpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef STATISTICSCOLLECTOR_HPP_
#define STATISTICSCOLLECTOR_HPP_

#include <utils/statistics/StatisticData.hpp>
#include <map>
#include <string>

using namespace std;

namespace Anakin {

class StatisticsCollector {
public:
	StatisticsCollector();
	virtual ~StatisticsCollector();

	void addItem(string command, double time);
	string compute();
private:
	map<string, StatisticData> items;
};

} /* namespace Anakin */

#endif /* STATISTICSCOLLECTOR_HPP_ */

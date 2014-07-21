/*
 * StatisticsCollector.cpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#include <logging/OutputPolicyFile.hpp>
#include <utils/statistics/StatisticsCollector.hpp>
#include <sstream>
#include <utility>

namespace Anakin {

StatisticsCollector::StatisticsCollector() {

}

void StatisticsCollector::addItem(string command, double time) {
	boost::mutex::scoped_lock l(StatisticsCollector::GetMutex());
	map<string, StatisticData>::iterator it = items.find(command);
	if (it != items.end()) {
		//element found;
		it->second.addTime(time);
	} else {
		//element NOT found;
		items.insert(make_pair(command, StatisticData(time)));
	}
}

string StatisticsCollector::compute() {
	boost::mutex::scoped_lock l(StatisticsCollector::GetMutex());
	std::stringstream output;
	for (map<string, StatisticData>::iterator it = items.begin();
			it != items.end(); ++it) {
		output << "Worst " << it->second.worstTime << " ms. | Best "
				<< it->second.bestTime << " ms. | Avg "
				<< it->second.getAvgTime() << " ms. | Executed "
				<< it->second.samples << " times | Command: " << it->first
				<< '\n';
	}
	return output.str();
}

StatisticsCollector::~StatisticsCollector() {
	// TODO Auto-generated destructor stub
}

} /* namespace Anakin */

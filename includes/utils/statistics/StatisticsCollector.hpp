/*
 * StatisticsCollector.hpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef STATISTICSCOLLECTOR_HPP_
#define STATISTICSCOLLECTOR_HPP_

#include <utils/statistics/StatisticData.hpp>
#include <boost/thread/pthread/mutex.hpp>
#include <map>
#include <string>

using namespace std;

namespace Anakin {

class StatisticsCollector {
public:
	StatisticsCollector();
	virtual ~StatisticsCollector();

	void addItem(string command, string group, double time);
	string compute();
private:
	map<string, StatisticData> items;
	map<string, StatisticData> groups;
	static boost::mutex& GetMutex() {
		static boost::mutex mutex;
		return mutex;
	}
};

} /* namespace Anakin */

#endif /* STATISTICSCOLLECTOR_HPP_ */

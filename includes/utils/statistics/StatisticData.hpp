/*
 * StatisticData.hpp
 *
 *  Created on: 18/7/2014
 *      Author: Franco Pellegrini
 */

#ifndef STATISTICDATA_HPP_
#define STATISTICDATA_HPP_

namespace Anakin {

class StatisticData {
public:
	long samples;
	double totalTime;
	double bestTime;
	double worstTime;

	StatisticData(double time);
	virtual ~StatisticData();

	void addTime(double time);
	double getAvgTime();
};

} /* namespace Anakin */

#endif /* STATISTICDATA_HPP_ */

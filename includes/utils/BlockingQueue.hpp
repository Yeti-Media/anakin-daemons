/*
 * BlockingQueue.hpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include <mutex>
#include <condition_variable>
#include <deque>

namespace Anakin {

template<typename T>
class BlockingQueue {
public:
	BlockingQueue();
	virtual ~BlockingQueue();

	void push(T const& value);
	T pop();
private:
	std::mutex d_mutex;
	std::condition_variable d_condition;
	std::deque<T> d_queue;
};

template<typename T>
BlockingQueue<T>::BlockingQueue() {
}

template<typename T>
BlockingQueue<T>::~BlockingQueue() {
}

template<typename T>
void BlockingQueue<T>::push(T const& value) {
	{
		std::unique_lock<std::mutex> lock(this->d_mutex);
		d_queue.push_front(value);
	}
	this->d_condition.notify_one();
}

template<typename T>
T BlockingQueue<T>::pop() {
	std::unique_lock<std::mutex> lock(this->d_mutex);
	this->d_condition.wait(lock, [=] {return !this->d_queue.empty();});
	T rc(std::move(this->d_queue.back()));
	this->d_queue.pop_back();
	return rc;
}

} /* namespace Anakin */

#endif /* BLOCKINGQUEUE_H_ */

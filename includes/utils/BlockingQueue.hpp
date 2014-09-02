/*
 * BlockingQueue.hpp
 *
 *  Created on: 29/05/2014
 *      Author: Franco Pellegrini
 */

#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include <sys/types.h>
#include <condition_variable>
#include <deque>
#include <mutex>

namespace Anakin {

template<typename T>
class BlockingQueue {
public:
	BlockingQueue();
	virtual ~BlockingQueue();

	void push(T const& value);
	T pop();
	bool empty();
	uint size();
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
	std::unique_lock<std::mutex> lock(this->d_mutex);
	d_queue.push_front(value);
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

template<typename T>
bool BlockingQueue<T>::empty() {
	return this->d_queue.empty();
}

template<typename T>
uint BlockingQueue<T>::size() {
	return this->d_queue.size();
}

} /* namespace Anakin */

#endif /* BLOCKINGQUEUE_H_ */

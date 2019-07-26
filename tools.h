#pragma once

#include <chrono>
#include <random>
#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <atomic>

//-------------------------------------------------------------Random number Generation

class Rand_Num_Gen {
public:
	Rand_Num_Gen();
	int randomGenerator(int lower_limit, int upper_limit);
private:
	std::uniform_int_distribution<int> dist;
	std::mt19937_64 the_generator;
	std::random_device ran_dev;
};

//----------------------------------------------------------------Measure time

class Time_Measure {
private:
	std::vector<std::chrono::high_resolution_clock::time_point> time_table;		//Holds all the time points recorded
public:
	void now();		//Logs the time when it is called
	void print(int step);	//Prints on the screen the difference between measurements for every <step> steps, Ex: step=2, 1-0,3-2,4-3,etc; Step=3: 2-0,5-3,8-5, etc;
	void print(int a, int b);	//Prints on the screen the difference for 2 specific time point logs; bigger-smaller
	int get_size();
};

//---------------------------------------------------------------Thread-safe queue

	template<typename T>			//Takes any data type
	class Thread_Safe_Queue {
	private:
		std::deque<T> queue;				//The actual queue
		std::mutex queue_mutex;				//Mutex to block
		std::condition_variable condition;	//Conditional to pause until more items have been added to the queue
	public:
		void push_back(T item)			//Simple push item at the end of the queue
		{
			std::unique_lock<std::mutex> queue_lock(queue_mutex);		//Lock the mutex for this function
			queue.push_back(item);						//Add item to the back of the queue
			condition.notify_one();						//Notify a thread that an item has been added to the queue
		}
		T pop_back()						//Simple get item from end of queue and remove from queue
		{
			std::unique_lock<std::mutex> queue_lock(queue_mutex);		//Lock the mutex for function
				while (queue.empty()) {										//If queue is empty
					condition.wait(queue_lock);								//Release mutex and wait until notified
				}
			auto item = queue.back();									//Get item
			queue.pop_back();											//Remove item from queue
			return item;
		}
		T pop_front()
		{
			std::unique_lock<std::mutex> queue_lock(queue_mutex);		//Lock the mutex for function
			while (queue.empty()) {										//If queue is empty
				condition.wait(queue_lock);								//Release mutex and wait until notified
			}
			auto item = queue.front();									//Get item
			queue.pop_front();											//Remove item from queue
			return item;
		}
		int check() {	//Returns the number of items in queue
			std::unique_lock<std::mutex> queue_lock(queue_mutex);
			return queue.size();
		}
	};

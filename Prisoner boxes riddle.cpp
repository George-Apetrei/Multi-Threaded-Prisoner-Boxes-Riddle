
#include <tools.h>

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

void fill_boxes(std::vector<int> &boxes, int n) {
	Rand_Num_Gen m;
	for (int i = 0; i < n; i++) {			//Fill new vector with contents of boxes
		boxes.push_back(i);
	}
	for (int i = 0; i < n; ++i) {			//Go through every box and swap its content with another random box from within the array
		std::swap(boxes[i], boxes[m.randomGenerator(0, n - 1)]);
	}
}

void display(std::vector<int> boxes) {
	std::cout << "object->box:\n";
	for (int i = 0; i < boxes.size(); i++) {
		std::cout << i << "->" << boxes[i] << std::endl;
	}
	std::cout << "Done\n";
}

/*
//This is the normal function, without any multi-threading, I left it here to compare the performance difference
double calculate2(std::vector<int> boxes) {		//Function to find the correct box for each
	int node;		//Marks the node you're at
	int True = 0, False = 0;	//Variables to hold the results
	int found = 0;		//1 or 0, if the number has been found or not
	for (int i = 0; i < boxes.size(); ++i) {		//Go through every box
		node = i;				//Mark the node it will start lookin gin
		for (int j = 0; j < (boxes.size() / 2); ++j) { //Search for the correct number in a maximum of half the total number of boxes
			if (i == boxes[node]) {		//If the number has been found inside the box you're looking
				True++;		//Marks as found
				found = 1;
				break;
			}
			else {						//If not found, go to the box belonging to the number inside
				node = boxes[node];
			}

		}

		if (found == 1) {		//If the number has not been found
			found = 0;
		}
	}
return 100 * ((double)True / boxes.size());
}
*/

int find_item(int item, std::vector<int>& boxes) {	//Function to search for a specific box in the boxes vector
	int node = item;	//Node is used to point towards the box to be looked into
	for (int j = 0; j < (boxes.size() / 2); ++j) { //Search for the correct number in a maximum of half the total number of boxes
		if (item == boxes[node]) {		//If the number has been found inside the box you're looking
			return 1;
		}
		else {						//If not found, go to the box belonging to the number inside
			node = boxes[node];
		}

	}
	return 0;		//If the item has not been found
}

double calculate(std::vector<int>& boxes) {	//Multi-threaded function to calcula
		auto num_threads = std::thread::hardware_concurrency();	//Find the number of threads on the machine
		Thread_Safe_Queue<int> q;
		std::vector<std::thread> threads;
		std::vector<double> results;
		//Fill thread-safe queue
		for (int i = 0; i < boxes.size(); ++i) {
			q.push_back(i);
		}

		//Prealocate results vector with 0
		for (int i = 0; i < num_threads; ++i) {
			results.push_back(0);
		}

		std::atomic_uint total_count{ boxes.size() };

		//For loop to create the consumer threads, makes total number of threads on machine, threads
		for (int i = 0; i < num_threads; ++i) {

			threads.push_back(std::thread([&total_count, &q, &results, i, &boxes]() {
				int item;
				while (true) {
					int iterations = --total_count;
					if (iterations < 0) break; // at this point there have been total_count pop_back operations done across all reader threads so we exit
					item = q.pop_back();
					// std::cout << item << "\n";
					results.at(i) += find_item(item, boxes);
				}
			}
			));
		}

		//Wait for  all threads to finish their workload
		for (int i = 0; i < threads.size(); ++i) {
			threads[i].join();
		}

//Sum up all the results
	auto sum = 0;
	for (int i = 0; i < results.size(); ++i) {
		sum += results[i];
	}
	//Returns the % correct
	return (100 * (double)sum) / boxes.size();
}

double random_finder(std::vector<int>& boxes) {
	Rand_Num_Gen m;
	std::vector<std::vector<int>> search_tree;	//This vector holds all the searches to be performed for every item
	//Loop to preallocate the space needed
	//Vector of vector for every item to be searched for, each vector inside the vector of vectors hold the boxes to be checked for the result
	for (int i = 0; i < boxes.size(); ++i) {
		search_tree.push_back(std::vector<int>());
		search_tree.at(i).reserve(boxes.size() / 2);
	}
	auto num_threads = std::thread::hardware_concurrency();
	std::atomic_uint iterator{ boxes.size() };
	std::vector<std::thread> threads;
	Thread_Safe_Queue<int> q;
	//Load the thread safe queue with all the indexes to be looked at
	for (int i = 0; i < boxes.size(); ++i) {
		q.push_back(i);
	}
	//Vector to create threads
	for (int i = 0; i < num_threads; ++i) {
		//Push back threads
		threads.push_back(std::thread([&]() {
			//The threads create a queue of boxes in which to search for each iterator
			//Ex: In search of 0, it will create the queue to look in the boxes 10, 15, 4, 55, etc...
			while (true) {
				int iteration = --iterator;//Atomic iteration to ensure the correct amount of queue as created
				if (iteration < 0) break;
				int queue = q.pop_front();//Get the item for which to create a queue from the thread_safe queue
				for (int j = 0; j < boxes.size() / 2; ++j) {	//Fill the queue
					int duplicate = 0;
					int item_to_insert = m.randomGenerator(0, boxes.size()-1);	//Get randomly generated item to insert
					for (int k = 0; k < search_tree.at(queue).size(); ++k) {	//Loop through already existing items
						if (item_to_insert == search_tree.at(queue).at(k)) {	//If duplicate is found, discard the iteration and start again, if not duplicate, add item to queue
							duplicate++;
							break;
						}
					}
					if (duplicate) {
						duplicate--;
						j--;
					}
					else {
						search_tree.at(queue).push_back(item_to_insert);
					}
				}
			}
		}));
	}

	for (int i = 0; i < num_threads; ++i) {
		threads.at(i).join();
	}

	iterator.store(boxes.size());	//Reset atomic iterator to go through all the vectors

	//Load thread safe queue once again
	for (int i = 0; i < boxes.size(); ++i) {
		q.push_back(i);
	}

	std::vector<int> results;	//This vector will hold, for each thread, the number of results that they have found
	//Preallocate the vector with a num_thread number of elements and initialize them all with 0
	for (int i = 0; i < num_threads; ++i) {
		results.push_back(0);
	}

	for (int i = 0; i < num_threads; ++i) {
		//Use already created threads
		threads.at(i) = std::thread([&iterator,i,&search_tree,&boxes,&q,&results]() {
			while (true) {
				int local_iterator = --iterator;
				if (local_iterator < 0) break;
				int line = q.pop_front();
				for (int j = 0; j < search_tree[line].size(); ++j) {	//Look inside the box that the queue of boxes is poiting at
					if (boxes.at(search_tree.at(line).at(j))==line) {	//If the item is found, mark as found and exit, else continue looking at the next element the queue is poiting at
						results.at(i)+=1;
						break;
					}
				}
			}
		});
	}

	//Wait for all threads
	for (int i = 0; i < num_threads; ++i) {
		threads.at(i).join();
	}

	int sum=0;
	for (int i = 0; i < results.size(); ++i) {
		sum += results[i];
	}
	return 100 * (double)sum / boxes.size();
}

int main() {

	//std::cout << "Threads: " << std::thread::hardware_concurrency();
	Rand_Num_Gen m;
	//Call m.randomGenerator(lower_limit, upper_limit) to use
	Time_Measure t;
	int n,sets;
	std::cout << "Number of boxes\n";
	std::cin >> n;
	std::cout << "Number of sets to be checked\n";
	std::cin >> sets;
	std::vector<std::vector<int>> boxes;
	std::vector<double> percent_correct,percent_correct_random;
	//Go through all the sets
	for (int i = 0; i < sets; ++i) {
		boxes.push_back(std::vector<int>());//Add vector
		fill_boxes(boxes.at(i), n);	//Fill and scramble vector
		percent_correct.push_back(calculate(boxes.at(i)));	//Using the algorithm search for items inside the boxes
		percent_correct_random.push_back(random_finder(boxes.at(i)));	//Search for items using a random search
	}
	int correct = 0, correct_random = 0;
	//Check how many sets have been 100% found
	for (int i = 0; i < percent_correct.size(); ++i) {
		if (percent_correct[i] == 100) {
			correct++;
		}
		if (percent_correct_random[i] == 100) {
			correct_random++;
		}
	}

	double average = 100*(double)correct / percent_correct.size();
	double average_random = 100 * (double)correct_random / percent_correct_random.size();
	std::cout << "Using the algorithm:\n";
	std::cout << "Out of " << sets << " sets of "<<n<<" numbers, there are " << correct << " sets where all the items have been found, average of " << average << "% sets where all the items have been found\n";
	std::cout << "Using randomness:\n";
	std::cout << "Out of " << sets << " sets of " << n << " numbers, there are " << correct_random << " sets where all the items have been found, average of " << average_random << "% sets where all the items have been found\n";
	std::cout << "Press enter to exit\n";
	std::getchar();
	std::getchar();
}


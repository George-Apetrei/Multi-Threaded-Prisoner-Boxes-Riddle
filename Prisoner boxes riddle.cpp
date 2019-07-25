// Prisoner boxes riddle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Header.h>

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>

/*
void fill_boxes(int *boxes, int n) {

	Rand_Num_Gen m;
	int generated, check = 0;
	for (int i = 0; i < n; ++i) {
		generated = m.randomGenerator(0, n - 1);
		for (int j = 0; j < i; ++j) {
			if (boxes[j] == generated) {
				check = 1;
				break;
			}
		}
		if (check == 0) {
			boxes[i] = generated;
		}
		else {
			check = 0;
			i--;
		}

	}
}
*/

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
void calculate(int *boxes, int n) {
	std::vector <int> path;
	int tru = 0, fals = 0, node, check = 0;

	for (int i = 0; i < n; ++i) {					//For every box
		node = boxes[i];
		for (int j = 0; j < n / 2; ++j) {			//Search in half of total number of boxes
			path.push_back(node);			//Push back the number of the box you're looking in
			if (i == node) {				//If the number inside the box is the same as the number of the box you're looking for
				tru++;						//Mark as found
				check = 1;
				break;
			}
			else {							//If not
				node = boxes[node];			//Put in the box the number
			}
		}
		if (check == 1) {
			for (int m = 0; m < path.size(); ++m) {
				if (m + 1 == path.size()) {
					std::cout << path[m] << std::endl;
				}
				else {
					std::cout << path[m] << "->";
				}

			}

		}
		else {
			fals++;
		}
		check = 0;
		path.clear();
	}
	std::cout << "True: " << tru << std::endl;
	std::cout << "False: " << fals << std::endl;
	std::cout << "Percentage correct: " << (100 * tru / n) << std::endl;
}

*/


//This is the normal function, without any multi-threading
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
	return 100 * (double)sum / boxes.size();
}

void random_finder(std::vector<int>& boxes) {
	Rand_Num_Gen m;
	std::vector<std::vector<int>> search_tree;	//This vector holds all the searches to be performed for every item
	for (int i = 0; i < boxes.size(); ++i) {
		search_tree.push_back(std::vector<int>());
		search_tree.at(i).reserve(boxes.size() / 2);
	}
	auto num_threads = std::thread::hardware_concurrency();
	std::atomic_uint iterator{ boxes.size() };
	std::vector<std::thread> threads;
	Thread_Safe_Queue<int> q;
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
					int item_to_insert = m.randomGenerator(0, boxes.size());	//Get randomly generated item to insert
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


}

int main() {

	//std::cout << "Threads: " << std::thread::hardware_concurrency();
	Rand_Num_Gen m;
	//Call m.randomGenerator(lower_limit, upper_limit) to use
	Time_Measure t;
	int n;
	std::cin >> n;
	std::vector<int> boxes;
	std::vector<double> percent_correct;
	t.now();
	for (int i = 0; i < 1; ++i) {
		fill_boxes(boxes, n);
		//display(boxes);
		random_finder(boxes);
		//percent_correct.push_back(calculate(boxes));

		//calculate2(boxes, percent_correct);

		boxes.clear();
	}
	t.now();
	t.print(2);
	int correct = 0, incorrect = 0;
	for (int i = 0; i < percent_correct.size(); ++i) {
		if (percent_correct[i] == 100) {
			correct++;
		}
		else {
			incorrect++;
		}
	}

	double average = 100*(double)correct / percent_correct.size();
	std::cout << "Out of " << percent_correct.size() << " sets of "<<n<<" numbers, there are" << correct << " sets where all the items have been found, average of " << average << " sets where all the items have been found\n";
	//random_finder(boxes, n);
	std::cin >> n;

}


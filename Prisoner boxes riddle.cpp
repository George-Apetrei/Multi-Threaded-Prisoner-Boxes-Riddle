// Prisoner boxes riddle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tools.h>

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

void fill_boxes(std::vector<int> &boxes,int n) {
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

/*
void calculate(std::vector<int> boxes,std::vector<double> &percent_correct) {		//Function to find the correct box for each
	
	//Functionality:
	//1-Start from the box marked with what you want to find (You want to find number 0, start at box 0)
	//2-If true, mark as found, end
	//3-If not true, go to the box corresponding to the number inside(Same as above but instead you find, for example 5, then you go to box 5 to check)
	//4-Repeat from step 2 for n/2 iterations
	

	int node;		//Marks the node you're at
	int True = 0, False = 0;	//Variables to hold the results
	int found = 0;		//1 or 0, if the number has been found or not
	for (int i = 0; i < boxes.size(); ++i) {		//Go through every box
		node = i;				//Mark the node it will start lookin gin
		std::cout << "Looking for " << i << std::endl;
		for (int j = 0; j < (boxes.size() / 2); ++j) { //Search for the correct number in a maximum of half the total number of boxes
			std::cout << " " <<node;
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
			std::cout << " FOUND!\n";
			found = 0;
		}
		else {					
			std::cout << " NOT found!\n";
		}

	}
	percent_correct.push_back((100 * ((double)True / boxes.size())));
	std::cout << "TRUE: " << True << "\nPercent Found: " << percent_correct[percent_correct.size()-1] << std::endl;
}
*/

int find_item(int item, std::vector<int>& boxes) {
	int node = item;
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

void function() {

}

void calculate(std::vector<int> boxes, std::vector<double> &percent_correct) {
	auto num_threads = std::thread::hardware_concurrency();	//Find the number of threads on the machine
	if (num_threads <= 1) { num_threads=2; } //If there is only 1 thread on the system, make this number 2, so that at least a producer and a consumer will be created

	Thread_Safe_Queue<int> q;
	std::vector<std::thread> threads;
	std::vector<int> results,runs;

	std::atomic<bool> is_running=false;
	
	//Producer thread
	threads.push_back(std::thread(
		[&]() {
		is_running = true;
		for (auto& i : boxes) {
			q.push_back(boxes[i]);
		}
		is_running = false;
	}
	));


	
	//For loop to create the consumer threads, makes total number of threads on machine - 1, threads
	for (int i = 0; i < num_threads-1; ++i) {
		results.push_back(0);
		runs.push_back(0);
		threads.push_back(std::thread([&is_running,&q,&results,i,&boxes,&runs]() {
			int item;
			 while (is_running || (q.check()!=0)) {
				 item = q.pop_back();
				// std::cout << item << "\n";
				results[i] += find_item(item, boxes);
				runs[i]++;
			}
		}
		));
	}


	//Wait for  all threads to finish their workload

	for (int i = 0; i < threads.size(); ++i) {
		threads[i].join();
	}

	auto sum=0,sum_runs=0;
	for (int i = 0; i < results.size();++i) {
		sum += results[i];
		sum_runs += runs[i];
	}
	
	percent_correct.push_back(results[0]);
}

void random_finder(int *boxes, int n) {
	Rand_Num_Gen m;
	std::vector <int> guesses;
	int tru = 0, fals = 0, check = 0, generated;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n / 2; ++j) {
			generated = m.randomGenerator(0, n - 1);
			for (int t = 0; t < guesses.size(); ++t) {
				if (guesses[t] == generated) {
					check = 1;
					break;
				}
			}
			if (check == 0) {
				guesses.push_back(generated);
			}
			else {
				check = 0;
				j--;
			}

		}
		for (int j = 0; j < guesses.size(); ++j) {
			if (guesses[j] == i) {
				tru++;
				check = 1;
				break;
			}
		}
		if (check == 0) {
			fals++;
		}
		else {
			check = 0;
		}
		guesses.clear();
	}
	std::cout << "True: " << tru << std::endl;
	std::cout << "False: " << fals << std::endl;
	std::cout << "Percentage correct: " << (100 * tru / n);
}

int main() {

	//std::cout << "Threads: " << std::thread::hardware_concurrency();
	Rand_Num_Gen m;
	//Call m.randomGenerator(lower_limit, upper_limit) to use
	int n;
	double average = 0;
	std::cin >> n;
	std::vector<int> boxes;
	std::vector<double> percent_correct;
	for (int i = 0; i < 1000; ++i) {
		fill_boxes(boxes, n);
		//display(boxes);
		calculate(boxes, percent_correct);
		boxes.clear();
	}
	int correct = 0, incorrect = 0;
	for (int i = 0; i < percent_correct.size(); ++i) {
		if (percent_correct[i] == 100) {
			correct++;
		}
		else {
			incorrect++;
		}
	}

	average = correct / percent_correct.size();
	//random_finder(boxes, n);
	std::cin >> n;

}


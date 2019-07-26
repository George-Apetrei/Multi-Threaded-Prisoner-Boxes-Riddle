
#include "tools.h"



//-------------------------------------------------------------Random number Generation


//Constructor for Rand_Num_Gen to create a seed for random generation when the function object is created
Rand_Num_Gen::Rand_Num_Gen() {
	the_generator.seed(ran_dev());
}

//The actual random generation function
int Rand_Num_Gen::randomGenerator(int lower_limit, int upper_limit) {						//Call this function to get an actual number lower_limit>= and <=upper_limit
	std::uniform_int_distribution<int>::param_type parameters(lower_limit, upper_limit);
	dist.param(parameters);
	return dist(the_generator);
}

//----------------------------------------------------------------Measure time

void Time_Measure::now()
{
	time_table.push_back(std::chrono::high_resolution_clock::now());
}

void Time_Measure::print(int step) {
	if (step <= 1) {
		std::cout << "Step is smaller or equal to 1!\n";
		return;
	}
	if (step > time_table.size()) {
		std::cout << "Step is bigger than size of recorded time points array!\n";
		return;
	}
	if (time_table.size() % step != 0) {
		std::cout << "Not all pairs are displayed!\n";
	}
	std::cout << "Numbers: " << time_table.size() << "\n";
	for (int i = 0; i < (time_table.size() / step); ++i) {
		std::cout << "Pair " << (i*step) << " - " << step * (1 + i) - 1 << ": " << std::chrono::duration_cast<std::chrono::microseconds>(time_table[step*(1 + i) - 1] - time_table[(i*step)]).count() << std::endl;
	}

}

void Time_Measure::print(int a, int b)
{
	if (a<0 || a>time_table.size() || b<0 || b>time_table.size()) {
		std::cout << "Invalid request!\n";
		return;
	}
	if (a > b) {	//Check which is bigger, then print
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_table[a] - time_table[b]).count() << std::endl;
	}
	else {
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_table[b] - time_table[a]).count() << std::endl;
	}

}

int Time_Measure::get_size()
{
	return time_table.size();
}
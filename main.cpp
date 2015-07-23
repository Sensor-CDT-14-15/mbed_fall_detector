#include <deque>
#include <math.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

float x, y, z;
deque<float> x_data, y_data, z_data;

// Calculates the mean of a deque
float average(deque<float> x) {
	deque<float>::iterator it = x.begin();
	float average;
	while (it != x.end()) {
		average += *it++;
	}
	average = average / x.size();
	return average;
}

// Calculates the population standard deviation of a deque
float stddev(deque<float> x) {
	deque<float>::iterator it = x.begin();
	float stddev;
	float average_x = average(x);
	while (it != x.end()) {
		float curr_x = *it++;
		stddev += pow(curr_x - average_x, 2);
	}
	stddev = sqrt(stddev / x.size());
	return stddev;
}

int main() {
	srand(time(NULL));
	x_data.assign(20, 0); y_data.assign(20, 0); z_data.assign(20, 0);

	for (int i = 0; i < 20; i++) {
		x = rand(); y = rand(); z = rand();
		printf("%f, %f, %f\n", x, y, z);
		x_data.push_front(x); y_data.push_front(x); z_data.push_front(x);
		x_data.pop_back(); y_data.pop_back(); z_data.pop_back();
	}

	deque<float>::iterator it = x_data.begin();
	while (it != x_data.end()) {
		printf("x_data element: %f\n", *it++);
	}
	printf("x_data average: %f\n", average(x_data));
	printf("x_data stddev: %f\n", stddev(x_data));
}

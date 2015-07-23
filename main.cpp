#include <deque>
#include <math.h>
#include <iostream>
#include <time.h>
#include <stdlib.h>

using namespace std;

deque<float> x_data, y_data, z_data;
deque<float> sigma;
deque<float> theta_x, theta_y, theta_z;

const float threshold_sig = 1.0105;
const float threshold_theta_z = 1.2639;
const float threshold_delta_theta_z = 0.8398;
const float threshold_ask_omar = 0.2050;
const float threshold_ask_omar_2 = 0.5905;

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

// Get x, y, z data and add to deques
void update_data() {
	for (int i = 0; i < 20; i++) {
		float x, y, z;
		x = rand(); y = rand(); z = rand();
		printf("%f, %f, %f\n", x, y, z);
		x_data.push_front(x); y_data.push_front(x); z_data.push_front(x);
		x_data.pop_back(); y_data.pop_back(); z_data.pop_back();

		float root = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		sigma.push_front(root);
		sigma.pop_back();
		theta_x.push_front(acos(x / root)); theta_y.push_front(acos(y / root)); theta_z.push_front(acos(z / root));
		theta_x.pop_back(); theta_y.pop_back(); theta_z.pop_back();
	}
}

bool fall_detected(float sig_ratio, float theta_z_ratio, float theta_z_delta, float sigA, float mag_sigXYZ) {
	bool fall = false;
	if (
		(sig_ratio > threshold_sig || theta_z_ratio > threshold_theta_z)
		&&
		(theta_z_ratio > threshold_theta_z || theta_z_delta > threshold_delta_theta_z)
		&&
		(sigA > threshold_ask_omar || mag_sigXYZ > threshold_ask_omar_2)
	) {
		printf("FALL\n");
		fall = true;
	} else {
		printf("NO FALL\n");
	}
	return fall;
}

template<class T>
deque<T> first_half(deque<T> x) {
	deque<T> first;
	first.assign(x.size() / 2, 0);
	for (int i = 0; i < first.size(); i++) {
		first[i] = x[i];
	}
	return first;
}

template<class T>
deque<T> last_half(deque<T> x) {
	deque<T> last;
	last.assign(x.size() - x.size() / 2, 0);
	for (int i = 0; i < last.size(); i++) {
		last[i] = x[x.size() / 2 + i];
	}
	return last;
}


int main() {
	srand(time(NULL));
	x_data.assign(20, 0); y_data.assign(20, 0); z_data.assign(20, 0);
	sigma.assign(20, 0);
	theta_x.assign(20, 0); theta_y.assign(20, 0); theta_z.assign(20, 0);

	update_data();

	float sigma_stddev_ratio = stddev(first_half(sigma)) / stddev(last_half(sigma));
	float theta_z_average_ratio = average(first_half(theta_z)) / average(first_half(theta_z));
	float theta_z_average_delta = average(first_half(theta_z)) - average(last_half(theta_z));

	fall_detected(0.0, 0.0, 0.0, 0.0, 0.0);
	fall_detected(1.01051, 1.26391, 0, 0.20501, 0);
}

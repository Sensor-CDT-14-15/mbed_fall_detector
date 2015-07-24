#include <vector>
#include <cmath>
#include <iostream>
#include "MMA8451Q.h"
#include <iterator>

using namespace std;

#define MMA8451_I2C_ADDRESS (0x1d<<1)
const PinName SDA = PTE25;
const PinName SCL = PTE24;
MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);

vector<float> x_data, y_data, z_data;
vector<float> sigma;
vector<float> theta_x, theta_y, theta_z;

const float threshold_sig                = 1.0105;
const float threshold_theta_z            = 1.2639;
const float threshold_delta_theta_z      = 0.8398;
const float threshold_sigma_stddev_first = 0.2050;
const float threshold_xyz_variances_norm = 0.5905;

const bool PRINT_DEBUG = false;

Timer t;

float average(vector<float> x) {
	vector<float>::iterator it = x.begin();
	float average = 0;
	while (it != x.end()) {
		average += *it++;
	}
	average = average / x.size();
	return average;
}

float stddev(vector<float> x) {
	vector<float>::iterator it = x.begin();
	float stddev = 0;
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
	t.start();
	for (int i = 0; i < 20; i++) {
		float x, y, z;
		x = abs(acc.getAccX()); y = abs(acc.getAccY()); z = abs(acc.getAccZ());
		if (PRINT_DEBUG) printf("x: %6.4f,   y: %6.4f,   z: %6.4f\n", x, y, z);
		x_data.push_back(x); y_data.push_back(x); z_data.push_back(x);
		x_data.erase(x_data.begin()); y_data.erase(y_data.begin()); z_data.erase(z_data.begin());

		float root = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
		sigma.push_back(root);
		sigma.erase(sigma.begin());
		theta_x.push_back(acos(x / root)); theta_y.push_back(acos(y / root)); theta_z.push_back(acos(z / root));
		theta_x.erase(theta_x.begin()); theta_y.erase(theta_y.begin()); theta_z.erase(theta_z.begin());
		wait(0.048);
	}
	t.stop();
	if (PRINT_DEBUG) printf("\nTime for data collection:    %6.4f\n", t.read());
	t.reset();
}

bool fall_detected(float sigma_stddev_ratio, float theta_z_average_ratio, float theta_z_average_delta, float sigma_stddev_first, float xyz_variances_norm) {
	bool fall = false;
	if (PRINT_DEBUG) {
		printf("\nsigma_stddev_ratio:         % 7.4f\n", sigma_stddev_ratio);
		printf("theta_z_average_ratio:      % 7.4f\n", theta_z_average_ratio);
		printf("theta_z_average_delta:      % 7.4f\n", theta_z_average_delta);
		printf("sigma_stddev_first:         % 7.4f\n", sigma_stddev_first);
		printf("xyz_variances_norm:         % 7.4f\n\n", xyz_variances_norm);
	}
	if (
		(sigma_stddev_ratio > threshold_sig || theta_z_average_ratio > threshold_theta_z)
		&&
		(theta_z_average_ratio > threshold_theta_z || theta_z_average_delta > threshold_delta_theta_z)
		&&
		(sigma_stddev_first > threshold_sigma_stddev_first || xyz_variances_norm > threshold_xyz_variances_norm)
	) {
		printf("FALL!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		fall = true;
	} else {
		printf("NO FALL\n");
	}
	return fall;
}

template<class T>
vector<T> first_half(vector<T> x) {
	vector<T> first;
	first.assign(x.size() / 2, 0);
	for (int i = 0; i < first.size(); i++) {
		first[i] = x[i];
	}
	return first;
}

template<class T>
vector<T> last_half(vector<T> x) {
	vector<T> last;
	last.assign(x.size() - x.size() / 2, 0);
	for (int i = 0; i < last.size(); i++) {
		last[i] = x[x.size() / 2 + i];
	}
	return last;
}


int main() {
	x_data.assign(20, 0); y_data.assign(20, 0); z_data.assign(20, 0);
	sigma.assign(20, 0);
	theta_x.assign(20, 0); theta_y.assign(20, 0); theta_z.assign(20, 0);

	while(1) {
		update_data();

		t.start();
		float sigma_stddev_ratio = stddev(first_half(sigma)) / stddev(last_half(sigma));
		float theta_z_average_ratio = average(first_half(theta_z)) / average(last_half(theta_z));
		float theta_z_average_delta = average(first_half(theta_z)) - average(last_half(theta_z));
		float sigma_stddev_first = stddev(first_half(sigma));
		float xyz_variances_norm = sqrt(pow(stddev(first_half(x_data)), 2) + pow(stddev(first_half(y_data)), 2) + pow(stddev(first_half(z_data)), 2));
		t.stop();
		if (PRINT_DEBUG) printf("Time for input calculations: %6.4f\n", t.read());
		t.reset();

		t.start();
		fall_detected(sigma_stddev_ratio, theta_z_average_ratio, theta_z_average_delta, sigma_stddev_first, xyz_variances_norm);
		t.stop();
		if (PRINT_DEBUG) printf("Time for decision:           %6.4f\n===================================\n", t.read());
		t.reset();
	}
}

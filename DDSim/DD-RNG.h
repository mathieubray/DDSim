/* ---------------------------------------------
DD-RNG.h
Defines a Random-Number Generator for Simulations
---------------------------------------------- */

#ifndef RNG_H
#define RNG_H

#include <math.h>

/* This uses a portable implementation for generating U(0,1) due to Schrage */
#define RAND_A 16807
#define RAND_M 2147483647
#define RAND_Q 127773
#define RAND_R 2836
#define RAND_SCALE (1.0 / RAND_M)

class RNG {

private:
	int seed; //Default seed is 1, which will be initialized in the constructor function;

public:
	RNG();
	~RNG();
	void setSeed(int s);				// s=0 is a bad choice;
	int getSeed();						// This will return a U(0,1) random variable;
	double runif();						// This will return a U(a,b) random variable;
	double runif(double l, double u);	// This will return an exponentially distributed random variable with hazard rate lambda;
	double rexp(double lambda);

};

RNG::RNG() {

	setSeed(1); // Could set seed according to random time
}

RNG::~RNG() {

}

void RNG::setSeed(int s) {

	if (s == 0)	// Cannot allow seed to be 0;
		seed = 1;
	else
		seed = s;
}

int RNG::getSeed() {

	return seed;
}

/* If this is working properly, starting with seed = 1,
the 10,000th call produces seed = 1043618065 */

double RNG::runif() {

	int k = seed / RAND_Q;
	seed = RAND_A * (seed - k* RAND_Q) - k * RAND_R;
	if (seed < 0)
		seed += RAND_M;
	return seed * (double)RAND_SCALE;
}

double RNG::runif(double l, double u) {

	return l + (u - l)*runif();
}

double RNG::rexp(double lambda) {

	return -log(runif()) / lambda;
}

#endif
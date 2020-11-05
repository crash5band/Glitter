#pragma once
#include <chrono>

class Stopwatch
{
private:
	using clock_t = std::chrono::high_resolution_clock;
	using second_t = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock_t> start;
	const char* name;

public:
	Stopwatch(const char* name);
	~Stopwatch();

	double elapsed();
	void reset();
};


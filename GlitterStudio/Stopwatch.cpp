#include "Stopwatch.h"
#include "Editor.h"

Stopwatch::Stopwatch()
{
	reset();
}

Stopwatch::~Stopwatch()
{

}

void Stopwatch::reset()
{
	begin = clock_t::now();
}

double Stopwatch::elapsed() const
{
	return std::chrono::duration_cast<second_t>(clock_t::now() - begin).count();
}
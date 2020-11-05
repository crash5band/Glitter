#include "Stopwatch.h"
#include "Editor.h"

Stopwatch::Stopwatch(const char* n) :
	name{ n }, start { clock_t::now() }
{

}

Stopwatch::~Stopwatch()
{
	Editor::times.emplace_back(DeltaTime{ name, elapsed() });
}

void Stopwatch::reset()
{
	start = clock_t::now();
}

double Stopwatch::elapsed()
{
	return std::chrono::duration_cast<second_t>(clock_t::now() - start).count();
}
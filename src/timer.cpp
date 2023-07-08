#include "Timer.h"
#include <cmath>

Timer::Timer() : start_time(std::chrono::steady_clock::now()) {}

void Timer::start()
{
    start_time = std::chrono::steady_clock::now();
}

double Timer::elapsed() const
{
    auto end_time = std::chrono::steady_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<double>>(end_time - start_time);
    return std::round(elapsed_time.count() * 1000.0) / 1000.0;
    ;
}

Timer Timer::create()
{
    return Timer();
}

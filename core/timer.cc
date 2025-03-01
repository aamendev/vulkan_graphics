#include "timer.h"
#include <chrono>
#include <thread>

namespace Lina { namespace Core {
    void Timer::begin()
    {
        mStart = std::chrono::steady_clock::now();
    }

    void Timer::end()
    {
       mEnd = std::chrono::steady_clock::now();
    }

    void Timer::wait(f32 time)
    {
        if (time < 0) return;
        auto initTime = std::chrono::high_resolution_clock::now();
        auto currentTime = initTime;
        do 
        {
             currentTime = std::chrono::high_resolution_clock::now();
             mWaitTime = currentTime - initTime; 
        } while ((mWaitTime).count() * 1000 < time);
    }

    f32 Timer::getTime(TimeUnit unit)
    {
        mCurrentTime = mEnd - mStart;
        return mCurrentTime.count() * 1000; 
    }

}}

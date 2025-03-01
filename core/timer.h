#ifndef TIMER_H
#define TIMER_H
#include "../types.h"
#include <chrono>
namespace Lina { namespace Core {
    enum class TimeUnit
    {
        Micro,
        Mili,
        Sec,
        Min,
        Hour,
    };
    class Timer
    {
        public:
        Timer() = default;
        void begin();
        void end();

        f32 getTime(TimeUnit unit = TimeUnit::Mili);
        void wait(f32 waitTime);
        private:
        std::chrono::time_point<std::chrono::steady_clock> mStart;
        std::chrono::time_point<std::chrono::steady_clock> mEnd;
        std::chrono::duration<f32> mCurrentTime;
        std::chrono::duration<f32> mWaitTime;
    };
}}
#endif

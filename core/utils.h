#ifndef CORE_UTILS_H
#define CORE_UTILS_H
#include "../types.h"
#include <cstdlib>
inline f32 randFloat(f32 a, f32 b)
{
    return a + ((f32)(rand() / 2) / (RAND_MAX / 2 + 1)) * (b - a);
}

#endif

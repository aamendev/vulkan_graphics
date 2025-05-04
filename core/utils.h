#ifndef CORE_UTILS_H
#define CORE_UTILS_H
#include "../types.h"
#include <cstdlib>
#include <vulkan/vulkan_core.h>

#define LERP(t, x0, x1) ((x0) + (t) * ((x1) - (x0)))
#define SMOOTH(x) ((x) * (x) * (x) * ((x) * ((x) * 6 - 15) + 10));

#define TABSIZE (256)
const int  perm[TABSIZE] =  {151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7,
    225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134,
    139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220,
    105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80,
    73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86,
    164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189,
    28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101,
    155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232,
    178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12,
    191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181,
    199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
    205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};


#define TABMASK (TABSIZE - 1)
#define PERM(x) perm[(x) & TABMASK]
#define INDEX(ix, iy, iz) PERM(ix + PERM(iy + PERM(iz)))
#define RANDMASK 0xffffffff
#define RANDNBR ((float)(rand() & RANDMASK) / RANDMASK) 

namespace Lina { namespace Core { namespace Utils {

    inline f32 randFloat(f32 a, f32 b)
    {
        return a + ((f32)(rand() / 2) / (RAND_MAX / 2 + 1)) * (b - a);
    }

    inline VkDescriptorType toVkBufferType(UniformType s)
    {
        return 
            (VkDescriptorType)(
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
                    * (s == UniformType::Static) + 
                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC  
                    * (s == UniformType::Dynamic));
    }

    inline int floor(float x)
    {
        int correction;
        if (x < 0 && x != int(x))
        {
            correction = 1;
        }
        else
        {
            correction = 0;
        }
        return (int)x - correction; 
    }

}}}
#endif

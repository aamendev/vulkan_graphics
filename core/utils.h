#ifndef CORE_UTILS_H
#define CORE_UTILS_H
#include "../types.h"
#include <cstdlib>
#include <vulkan/vulkan_core.h>
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
}}}
#endif

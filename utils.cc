#include "./utils.h"

namespace Lina{ namespace Graphics{
    namespace Utils
    {
        u32 findMemoryType(
                VkPhysicalDevice device,
                u32 typeFilter,
                VkMemoryPropertyFlags flags)
        {
            VkPhysicalDeviceMemoryProperties memProperties;
            vkGetPhysicalDeviceMemoryProperties(device, &memProperties);
            for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
                if ((typeFilter & (1 << i))
                        && (memProperties.memoryTypes[i].propertyFlags & flags) == flags) {
                    return i;
                }
            }
            throw std::runtime_error("failed to find suitable memory type!");
        }
    }
}}

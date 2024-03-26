#pragma once
#include "./common.h"
namespace Lina{ namespace Graphics{
    namespace Utils
    {
        u32 findMemoryType(VkPhysicalDevice device, u32 typeFilter, VkMemoryPropertyFlags flags);

        /* Temprary */
        //void transitionImageLayout(VkCommandPool commandPool, VkDevice device, VkQueue graphicsQUeue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        /* End Temporary */
    }
}}

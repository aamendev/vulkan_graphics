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

//        void transitionImageLayout(VkCommandPool commandPool, VkDevice device, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
//        {
//            VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool, device);
//
//            endSingleTimeCommand(commandPool, device, graphicsQueue, commandBuffer);
//
//
//            VkImageMemoryBarrier barrier =
//            {
//                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
//                .srcAccessMask = 0,
//                .dstAccessMask = 0,
//                .oldLayout = oldLayout,
//                .newLayout = newLayout,
//                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//                .image = image,
//                .subresourceRange =
//                {
//                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
//                    .baseMipLevel = 0,
//                    .levelCount = 1,
//                    .baseArrayLayer = 0,
//                    .layerCount = 1
//                }
//            };
//
//            vkCmdPipelineBarrier(
//                    commandBuffer,
//                    0,
//                    0,
//                    0,
//                    0, nullptr,
//                    0, nullptr,
//                    1, &barrier
//                    );
//        }
    }
}}

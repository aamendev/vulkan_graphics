#pragma once
#include "common.h"
#include <vulkan/vulkan_core.h>
namespace Lina{ namespace Graphics{
    struct DeviceSpecs
    {
        VkInstance* instance;
        std::vector<VkPhysicalDevice> physicalDevices;

        std::vector
        <
            std::vector<VkQueueFamilyProperties>
        > queueFamilyProperties;

        VkPhysicalDevice physicalDevice;
        VkDevice device;
        const std::vector<const char *> requiredExensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        };
        SwapChainSupportDetails swapChainDetails;
        QueueFamilyIndices familyIndices;
        VkSurfaceKHR surface;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkPhysicalDeviceLimits limits;
    };
    class DeviceHandler
    {
        friend class VulkanInitializer;
        friend class Renderer;
        friend class SwapChain;

        friend class Buffer;
        friend class VertexBuffer;
        friend class IndexBuffer;
        friend class UniformBuffer;
        friend class StagingBuffer;

        friend class Texture;
        friend class Image;
        friend class TextureImage;

        public:
            b8 init(VkInstance* mInstance, VkSurfaceKHR* surface);
        private:

            b8 setVulkanDevices();
            b8 setQueueFamilyProperties();
            b8 setCurrentDevice();
            b8 createLogicalDevice();

            //helpers
            b8 isDeviceSuitable(VkPhysicalDevice, i32);
            b8 familyWithGraphicsBit(VkPhysicalDevice device ,i32 index);
            b8 checkRequiredExtensions(VkPhysicalDevice device);
            SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
        private:
            DeviceSpecs mSpecs;
    };
}}

#pragma once
#include "common.h"
namespace Lina{ namespace Graphics{
    struct specs
    {
        VkInstance* vInstance;
        std::vector<VkPhysicalDevice> vPhysicalDevices;

        std::vector
        <
            std::vector<VkQueueFamilyProperties>
        > vQueueFamilyProperties;

        VkPhysicalDevice vPhysicalDevice;
        VkDevice vDevice;
        const std::vector<const char *> vRequiredExensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        SwapChainSupportDetails vSwapChainDetails;
        QueueFamilyIndices vFamilyIndices;
        VkSurfaceKHR vSurface;
        VkQueue vGraphicsQueue;
        VkQueue vPresentQueue;
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
            b8 init(VkInstance* vInstance, VkSurfaceKHR* surface);
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
            specs vSpecs;
    };
}}

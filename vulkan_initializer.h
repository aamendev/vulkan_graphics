#pragma once
#include "common.h"
#include "window.h"
#include "shader.h"
#include "devices_handler.h"
#include "swap_chain.h"
#include "./vertex_buffer.h"
#include "./vertex_buffer_layout.h"
#include <glm/glm.hpp>
#include <array>
namespace Lina{ namespace Graphics{
    struct VulkanSpecs
    {
        VkInstance vInstance;

        VkSurfaceKHR vSurface;

        SwapChainSupportDetails vSwapChainDetails;

        VkCommandPool vCommandPool;
        VkCommandBuffer vCommandBuffer;
    };
    class VulkanInitializer
    {
        friend class Renderer;
        public:
            b8 init(std::string& appName, Window* window);


            void clean();
            /* Getters */
            VulkanSpecs* getVulkanSpecs();

        private:
            b8 createVulkanInstance(std::string& appName);

            void createWindowSurface();

            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkPresentModeKHR chooseSwapPresentMode();
            VkExtent2D chooseSwapExtent();


            void createGraphicsPipeline();


            void createCommandPool();
            void createCommandBuffer();
            void createVertexBuffer();


            void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags,
                    VkBuffer&, VkDeviceMemory&);

            u32 findMemoryType(u32, VkMemoryPropertyFlags);
        private:
            VulkanSpecs vSpecs;
            Window* vWindow;
            DeviceHandler* vDeviceHandler;
            SwapChain* vSwapChain;
    };
}}

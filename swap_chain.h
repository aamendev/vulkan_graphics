#pragma once
#include "common.h"
#include "devices_handler.h"
#include "window.h"
#include "./depth_image.h"
namespace Lina{ namespace Graphics{
    struct SwapChainSpecs
    {
        VkSurfaceFormatKHR format;
        VkPresentModeKHR presentMode;
        VkExtent2D extent;
        VkSwapchainKHR swapChain;

        std::vector<VkImage> swapImages;
        std::vector<VkImageView> swapImageViews;

        DepthImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFrameBuffers;

        VkSemaphore imageAvailableSemaphore;
        VkSemaphore renderFinishedSemaphore;
        VkFence inFlightFence;

        SwapChainSupportDetails* swapChainDetails;
        VkSurfaceKHR* surface;
    };
    class SwapChain
    {
        friend class VulkanInitializer;
        friend class Renderer;
        public:
            b8 init(SwapChainSupportDetails*, VkSurfaceKHR*, Window*, DeviceHandler*);
            void recreate();
        private:
            b8 createSwapChain();
            b8 createDepthResources();
            b8 createImageViews();
            b8 createRenderPass();
            b8 createFramebuffers();
            b8 createSyncObjects();
            void clean();

            //helpers
            VkSurfaceFormatKHR chooseSwapSurfaceFormat();
            VkPresentModeKHR chooseSwapPresentMode();
            VkExtent2D chooseSwapExtent();
        private:
            SwapChainSpecs mSpecs;
            DeviceHandler* mDeviceHandler;
            Window* mWindow;
    }; }}

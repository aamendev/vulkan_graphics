#pragma once
#include "common.h"
#include "devices_handler.h"
#include "window.h"
namespace Lina{ namespace Graphics{
    struct SwapChainSpecs
    {
        VkSurfaceFormatKHR vFormat;
        VkPresentModeKHR vPresentMode;
        VkExtent2D vExtent;
        VkSwapchainKHR vSwapChain;
        std::vector<VkImage> vSwapImages;
        std::vector<VkImageView> vSwapImageViews;
        VkRenderPass vRenderPass;
        std::vector<VkFramebuffer> vSwapChainFrameBuffers;

        VkSemaphore vImageAvailableSemaphore;
        VkSemaphore vRenderFinishedSemaphore;
        VkFence vInFlightFence;

        SwapChainSupportDetails* vSwapChainDetails;
        VkSurfaceKHR* vSurface;
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
            SwapChainSpecs vSpecs;
            DeviceHandler* vDeviceHandler;
            Window* vWindow;
    };
}}

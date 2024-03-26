#include "vulkan_initializer.h"
#include <glm/glm.hpp>
#include <array>
#include <cstring>

namespace Lina{ namespace Graphics{
    b8 VulkanInitializer::init(std::string& name, Window* window)
    {
        vWindow = window;
        vDeviceHandler = new DeviceHandler();
        vSwapChain = new SwapChain();
        if (createVulkanInstance(name))
        {
            createWindowSurface();

            vDeviceHandler->init(&vSpecs.vInstance, &vSpecs.vSurface);

            vSpecs.vSwapChainDetails = vDeviceHandler->vSpecs.vSwapChainDetails;

            vSwapChain->init(&vSpecs.vSwapChainDetails, &vSpecs.vSurface, window, vDeviceHandler);

            createCommandPool();
            createCommandBuffer();
            return true;
        }
        return false;
    }
    /*void VulkanInitializer::clean()
    {
        vkDestroySemaphore(vDeviceHandler->vSpecs.vDevice, vSpecs.vImageAvailableSemaphore, nullptr);
        vkDestroySemaphore(vDeviceHandler->vSpecs.vDevice, vSpecs.vRenderFinishedSemaphore, nullptr);
        vkDestroyFence(vDeviceHandler->vSpecs.vDevice, vSpecs.vInFlightFence, nullptr);
        for (auto frameBuffer : vSpecs.vSwapChainFrameBuffers)
        {
            vkDestroyFramebuffer(vDeviceHandler->vSpecs.vDevice, frameBuffer, nullptr);
        }
        vkDestroyPipeline(vDeviceHandler->vSpecs.vDevice, vSpecs.vGraphicsPipeline, nullptr);
        vkDestroyPipelineLayout(vDeviceHandler->vSpecs.vDevice, vSpecs.vPipelineLayout, nullptr);
        vkDestroyRenderPass(vDeviceHandler->vSpecs.vDevice, vSpecs.vRenderPass, nullptr);

        for (auto imageView : vSpecs.vSwapImageViews) {
            vkDestroyImageView(vDeviceHandler->vSpecs.vDevice, imageView, nullptr);
        }

        vkDestroySwapchainKHR(vDeviceHandler->vSpecs.vDevice, vSpecs.vSwapChain, nullptr);
        vkDestroyDevice(vDeviceHandler->vSpecs.vDevice, nullptr);

        vkDestroySurfaceKHR(vSpecs.vInstance, vSpecs.vSurface, nullptr);
        vkDestroyInstance(vSpecs.vInstance, nullptr);


    }*/
    b8 VulkanInitializer::createVulkanInstance(std::string& name)
    {
        VkApplicationInfo appInfo
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = name.c_str(),
                .applicationVersion = VK_MAKE_VERSION(1, 3, 0),
                .pEngineName = "No Engine",
                .engineVersion = VK_MAKE_VERSION(1,0,0),
                .apiVersion = VK_API_VERSION_1_3
        };
        u32 glfwExtensionCount = 0;
        const char** glfwExtensions =
            glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        VkInstanceCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                .pApplicationInfo = &appInfo,
                .enabledExtensionCount = glfwExtensionCount,
                .ppEnabledExtensionNames = glfwExtensions,
        };
        if (vkCreateInstance(&createInfo,nullptr, &(vSpecs.vInstance)) != VK_SUCCESS)
        {
            std::cout<<"Failed to create Instance\n";
            return false;
        }
        return true;
    }


    void VulkanInitializer::createCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = (u32)vDeviceHandler->vSpecs.vFamilyIndices.graphicsFamily
        };
        vkCreateCommandPool(vDeviceHandler->vSpecs.vDevice, &poolInfo, nullptr, &(vSpecs.vCommandPool));
    }
    void VulkanInitializer::createCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = vSpecs.vCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        vkAllocateCommandBuffers(vDeviceHandler->vSpecs.vDevice, &allocInfo, &(vSpecs.vCommandBuffer));
    }

    void VulkanInitializer::createWindowSurface()
    {
        vWindow->createWindowSurface(vSpecs.vInstance, &(vSpecs.vSurface));
    }
    VulkanSpecs* VulkanInitializer::getVulkanSpecs()
    {
        return &vSpecs;
    }
}}

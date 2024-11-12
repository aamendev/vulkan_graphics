#include "devices_handler.h"
#include <cstring>
#include <vulkan/vulkan_core.h>
namespace Lina{ namespace Graphics{
    b8 DeviceHandler::init(VkInstance* instance, VkSurfaceKHR* surface)
    {
        mSpecs.instance = instance;
        mSpecs.surface = *surface;
        return
            setVulkanDevices()
            && setQueueFamilyProperties()
            && setCurrentDevice()
            && createLogicalDevice();
    }

    b8 DeviceHandler::setVulkanDevices()
    {
        u32 physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(
                *mSpecs.instance,
                &physicalDeviceCount ,
                nullptr
                );
        mSpecs.physicalDevices.resize(physicalDeviceCount);
        mSpecs.queueFamilyProperties.resize(physicalDeviceCount);
        return vkEnumeratePhysicalDevices(*mSpecs.instance, &physicalDeviceCount, &(mSpecs.physicalDevices[0])) == VK_SUCCESS;
    }

    b8 DeviceHandler::setQueueFamilyProperties()
    {
        u32 queueFamilyPropertiesCount;
        VkPhysicalDeviceMemoryProperties pDeviceProperties;
        u32 index = 0;
        for (auto d : mSpecs.physicalDevices)
        {
            vkGetPhysicalDeviceMemoryProperties(d, &pDeviceProperties);

            vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyPropertiesCount, nullptr);
            mSpecs.queueFamilyProperties[index].resize(queueFamilyPropertiesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(
                    d,
                    &queueFamilyPropertiesCount,
                    mSpecs.queueFamilyProperties[index++].data());
        }
        return true;
    }

    b8 DeviceHandler::setCurrentDevice()
    {
        i32 index = 0;
        for (const auto& d : mSpecs.physicalDevices)
        {
            if (isDeviceSuitable(d, index++))
            {
                mSpecs.physicalDevice = d;
                return true;
            }
        }
        return false;
    }

    b8 DeviceHandler::isDeviceSuitable(VkPhysicalDevice device, i32 index)
    {
        b8 extensionSupported = checkRequiredExtensions(device);
        b8 swapChainAdequate = false;
        if (extensionSupported)
        {
            mSpecs.swapChainDetails = querySwapChainSupport(device);
            swapChainAdequate =
                !mSpecs.swapChainDetails.formats.empty() &&
                !mSpecs.swapChainDetails.presnetModes.empty();
        }
        return familyWithGraphicsBit(device, index) &&
            extensionSupported &&
            swapChainAdequate;
    }
    b8 DeviceHandler::familyWithGraphicsBit(VkPhysicalDevice device ,i32 index)
    {
        int currentIndex = 0;
        for (auto& f : mSpecs.queueFamilyProperties[index])
        {
            if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                mSpecs.familyIndices.graphicsFamily = currentIndex;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, currentIndex, mSpecs.surface, &presentSupport);
            if (presentSupport)
            {
                mSpecs.familyIndices.presentFamily = currentIndex;
            }
            if (mSpecs.familyIndices.isComplete())
            {
                return true;
            }
            currentIndex++;
        }
        return false;
    }
    b8 DeviceHandler::checkRequiredExtensions(VkPhysicalDevice device)
    {
        u32 extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);

        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions
            (
             mSpecs.requiredExensions.begin(),
             mSpecs.requiredExensions.end()
            );
        for (const auto& e: availableExtensions)
        {
            requiredExtensions.erase(e.extensionName);
        }
        return requiredExtensions.empty();
    }
    SwapChainSupportDetails DeviceHandler::querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR
            (
             device,mSpecs.surface,
             &details.capabilities
            );
        u32 formatCount;

        vkGetPhysicalDeviceSurfaceFormatsKHR
            (
             device,
             mSpecs.surface,
             &formatCount,
             nullptr
            );

        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR
            (
             device,
             mSpecs.surface,
             &formatCount,
             details.formats.data()
            );

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR
            (
             device,
             mSpecs.surface,
             &presentModeCount,
             nullptr
            );

        details.presnetModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfacePresentModesKHR
            (
             device,
             mSpecs.surface,
             &presentModeCount,
             details.presnetModes.data()
            );
        return details;
    }
    b8 DeviceHandler::createLogicalDevice()
    {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<u32> uniqueQueueFamilies=
        {
            (u32)mSpecs.familyIndices.graphicsFamily,
            (u32)mSpecs.familyIndices.presentFamily
        };

        f32 priority = 1.0f;
        for (u32 queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .queueFamilyIndex = queueFamily,
                    .queueCount = 1,
                    .pQueuePriorities = &priority,
            };
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures features = {
            .depthClamp = VK_TRUE,
            .samplerAnisotropy = VK_TRUE,
        };
        VkDeviceCreateInfo deviceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount = (u32)queueCreateInfos.size(),
                .pQueueCreateInfos = queueCreateInfos.data(),
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = (u32)mSpecs.requiredExensions.size(),
                .ppEnabledExtensionNames = mSpecs.requiredExensions.data(),
                .pEnabledFeatures = &features
        };

        vkCreateDevice(
                mSpecs.physicalDevice,
                &deviceCreateInfo,
                nullptr,
                &(mSpecs.device)
                );

        vkGetDeviceQueue(
                mSpecs.device,
                (u32)mSpecs.familyIndices.graphicsFamily,
                0,
                &(mSpecs.graphicsQueue)
                );
        vkGetDeviceQueue(
                mSpecs.device,
                (u32)mSpecs.familyIndices.presentFamily,
                0,
                &(mSpecs.presentQueue)
                );
        return true;
    }
}}

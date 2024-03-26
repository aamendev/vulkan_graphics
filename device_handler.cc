#include "devices_handler.h"
namespace Lina{ namespace Graphics{
    b8 DeviceHandler::init(VkInstance* instance, VkSurfaceKHR* surface)
    {
        vSpecs.vInstance = instance;
        vSpecs.vSurface = *surface;
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
                *vSpecs.vInstance,
                &physicalDeviceCount ,
                nullptr
                );
        vSpecs.vPhysicalDevices.resize(physicalDeviceCount);
        vSpecs.vQueueFamilyProperties.resize(physicalDeviceCount);
        return vkEnumeratePhysicalDevices(*vSpecs.vInstance, &physicalDeviceCount, &(vSpecs.vPhysicalDevices[0])) == VK_SUCCESS;
    }

    b8 DeviceHandler::setQueueFamilyProperties()
    {
        u32 queueFamilyPropertiesCount;
        VkPhysicalDeviceMemoryProperties pDeviceProperties;
        u16 index = 0;
        for (auto d : vSpecs.vPhysicalDevices)
        {
            vkGetPhysicalDeviceMemoryProperties(d, &pDeviceProperties);

            vkGetPhysicalDeviceQueueFamilyProperties(d, &queueFamilyPropertiesCount, nullptr);
            vSpecs.vQueueFamilyProperties[index].resize(queueFamilyPropertiesCount);
            vkGetPhysicalDeviceQueueFamilyProperties(
                    d,
                    &queueFamilyPropertiesCount,
                    vSpecs.vQueueFamilyProperties[index++].data());
        }
        return true;
    }

    b8 DeviceHandler::setCurrentDevice()
    {
        i32 index = 0;
        for (const auto& d : vSpecs.vPhysicalDevices)
        {
            if (isDeviceSuitable(d, index++))
            {
                vSpecs.vPhysicalDevice = d;
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
            vSpecs.vSwapChainDetails = querySwapChainSupport(device);
            swapChainAdequate =
                !vSpecs.vSwapChainDetails.formats.empty() &&
                !vSpecs.vSwapChainDetails.presnetModes.empty();
        }
        return familyWithGraphicsBit(device, index) &&
            extensionSupported &&
            swapChainAdequate;
    }
    b8 DeviceHandler::familyWithGraphicsBit(VkPhysicalDevice device ,i32 index)
    {
        int currentIndex = 0;
        for (auto& f : vSpecs.vQueueFamilyProperties[index])
        {
            if (f.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                vSpecs.vFamilyIndices.graphicsFamily = currentIndex;
            }
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, currentIndex, vSpecs.vSurface, &presentSupport);
            if (presentSupport)
            {
                vSpecs.vFamilyIndices.presentFamily = index;
            }
            if (vSpecs.vFamilyIndices.isComplete())
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
             vSpecs.vRequiredExensions.begin(),
             vSpecs.vRequiredExensions.end()
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
             device,vSpecs.vSurface,
             &details.capabilities
            );
        u32 formatCount;

        vkGetPhysicalDeviceSurfaceFormatsKHR
            (
             device,
             vSpecs.vSurface,
             &formatCount,
             nullptr
            );

        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR
            (
             device,
             vSpecs.vSurface,
             &formatCount,
             details.formats.data()
            );

        u32 presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR
            (
             device,
             vSpecs.vSurface,
             &presentModeCount,
             nullptr
            );

        details.presnetModes.resize(presentModeCount);

        vkGetPhysicalDeviceSurfacePresentModesKHR
            (
             device,
             vSpecs.vSurface,
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
            (u32)vSpecs.vFamilyIndices.graphicsFamily,
            (u32)vSpecs.vFamilyIndices.presentFamily
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

        VkPhysicalDeviceFeatures features = {};
        VkDeviceCreateInfo deviceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                .queueCreateInfoCount = (u32)queueCreateInfos.size(),
                .pQueueCreateInfos = queueCreateInfos.data(),
                .enabledLayerCount = 0,
                .ppEnabledLayerNames = nullptr,
                .enabledExtensionCount = (u32)vSpecs.vRequiredExensions.size(),
                .ppEnabledExtensionNames = vSpecs.vRequiredExensions.data(),
                .pEnabledFeatures = &features
        };

        vkCreateDevice(
                vSpecs.vPhysicalDevice,
                &deviceCreateInfo,
                nullptr,
                &(vSpecs.vDevice)
                );

        vkGetDeviceQueue(
                vSpecs.vDevice,
                (u32)vSpecs.vFamilyIndices.graphicsFamily,
                0,
                &(vSpecs.vGraphicsQueue)
                );
        vkGetDeviceQueue(
                vSpecs.vDevice,
                (u32)vSpecs.vFamilyIndices.presentFamily,
                0,
                &(vSpecs.vPresentQueue)
                );
        return true;
    }
}}

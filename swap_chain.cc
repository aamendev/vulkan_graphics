#include "swap_chain.h"
namespace Lina{ namespace Graphics{
    b8 SwapChain::init(SwapChainSupportDetails* details, VkSurfaceKHR* surface,
            Window* window, DeviceHandler* d)
    {
        vSpecs.vSwapChainDetails = details;
        vSpecs.vSurface = surface;
        vWindow = window;
        vDeviceHandler = d;
        return
            createSwapChain()
            && createImageViews()
            && createRenderPass()
            && createFramebuffers()
            && createSyncObjects();
    }

    b8 SwapChain::createSwapChain()
    {
        vSpecs.vFormat = chooseSwapSurfaceFormat();
        vSpecs.vPresentMode = chooseSwapPresentMode();
        vSpecs.vExtent = chooseSwapExtent();

        u32 imageCount = vSpecs.vSwapChainDetails->capabilities.minImageCount + 1;
        if (vSpecs.vSwapChainDetails->capabilities.maxImageCount > 0
                && imageCount > vSpecs.vSwapChainDetails->capabilities.maxImageCount)
        {
            imageCount = vSpecs.vSwapChainDetails->capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = *(vSpecs.vSurface),
                .minImageCount = imageCount,
                .imageFormat = vSpecs.vFormat.format,
                .imageColorSpace = vSpecs.vFormat.colorSpace,
                .imageExtent = vSpecs.vExtent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
        };

        u32 queueFamilyIndices[] =
        {
            (u32)vDeviceHandler->vSpecs.vFamilyIndices.graphicsFamily,
            (u32)vDeviceHandler->vSpecs.vFamilyIndices.presentFamily
        };
        if (vDeviceHandler->vSpecs.vFamilyIndices.graphicsFamily != vDeviceHandler->vSpecs.vFamilyIndices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }
        createInfo.preTransform = vSpecs.vSwapChainDetails->capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = vSpecs.vPresentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;
        vkCreateSwapchainKHR(vDeviceHandler->vSpecs.vDevice, &createInfo, nullptr, &(vSpecs.vSwapChain));

        vkGetSwapchainImagesKHR(vDeviceHandler->vSpecs.vDevice, vSpecs.vSwapChain, &imageCount, nullptr);
        vSpecs.vSwapImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vDeviceHandler->vSpecs.vDevice, vSpecs.vSwapChain, &imageCount, vSpecs.vSwapImages.data());
        return true;
    }

    b8 SwapChain::createImageViews()
    {
        vSpecs.vSwapImageViews.resize(vSpecs.vSwapImages.size());
        for (u32 i = 0; i < vSpecs.vSwapImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = vSpecs.vSwapImages[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = vSpecs.vFormat.format,
            };
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            vkCreateImageView(vDeviceHandler->vSpecs.vDevice, &createInfo, nullptr, &(vSpecs.vSwapImageViews[i]));
        }
        return true;
    }

    b8 SwapChain::createRenderPass()
    {
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        VkAttachmentDescription colorAttachement
        {
            .format = vSpecs.vFormat.format,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        };
        VkAttachmentReference colorAttachementReference
        {
            .attachment = 0,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        VkSubpassDescription subpass
        {
            .colorAttachmentCount = 1,
                .pColorAttachments = &colorAttachementReference
        };

        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = 1,
                .pAttachments = &colorAttachement,
                .subpassCount = 1,
                .pSubpasses = &subpass
        };
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        return vkCreateRenderPass(
                vDeviceHandler->vSpecs.vDevice,
                &renderPassInfo,
                nullptr,
                &(vSpecs.vRenderPass)) == VK_SUCCESS;
    }
    b8 SwapChain::createFramebuffers()
    {
        vSpecs.vSwapChainFrameBuffers.resize(vSpecs.vSwapImageViews.size());
        for (size_t i = 0; i < vSpecs.vSwapImageViews.size(); i++) {
            VkImageView attachments[] = {
                vSpecs.vSwapImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = vSpecs.vRenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = vSpecs.vExtent.width;
            framebufferInfo.height = vSpecs.vExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(
                        vDeviceHandler->vSpecs.vDevice,
                        &framebufferInfo,
                        nullptr,
                        &(vSpecs.vSwapChainFrameBuffers[i])) != VK_SUCCESS) {
                return false;
            }
        }
        return true;
    }

    b8 SwapChain::createSyncObjects()
    {
        VkSemaphoreCreateInfo semaphoreInfo
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VkFenceCreateInfo fenceInfo
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT
        };

        if (vkCreateSemaphore(
                vDeviceHandler->vSpecs.vDevice,
                &semaphoreInfo,
                nullptr,
                &(vSpecs.vImageAvailableSemaphore)) != VK_SUCCESS)
        {
            return false;
        }

        if(vkCreateSemaphore(
                vDeviceHandler->vSpecs.vDevice,
                &semaphoreInfo,
                nullptr,
                &(vSpecs.vRenderFinishedSemaphore)) != VK_SUCCESS)
        {
            return false;
        }

        if(vkCreateFence(
                vDeviceHandler->vSpecs.vDevice,
                &fenceInfo,
                nullptr,
                &(vSpecs.vInFlightFence)) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }
    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat()
    {
        for (const auto& availableFormat : vSpecs.vSwapChainDetails->formats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                    && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return vSpecs.vSwapChainDetails->formats[0];
    }
    VkPresentModeKHR SwapChain::chooseSwapPresentMode()
    {
        for (const auto& availablePresentMode : vSpecs.vSwapChainDetails->presnetModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }
    VkExtent2D SwapChain::chooseSwapExtent()
    {
        VkSurfaceCapabilitiesKHR cap = vSpecs.vSwapChainDetails->capabilities;

        i32 width, height;
        glfwGetFramebufferSize(vWindow->getWindow(), &width, &height);
        VkExtent2D actualExtent =
        {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };
        return actualExtent;
    }

    void SwapChain::recreate()
    {
        vkDeviceWaitIdle(vDeviceHandler->vSpecs.vDevice);
        clean();
        createSwapChain();
        createImageViews();
        createFramebuffers();
    }

    void SwapChain::clean()
    {
        for (u32 i = 0; i < vSpecs.vSwapChainFrameBuffers.size(); i++)
        {
            vkDestroyFramebuffer(
                    vDeviceHandler->vSpecs.vDevice,
                    vSpecs.vSwapChainFrameBuffers[i],
                    nullptr
                    );
        }

        for (u32 i = 0; i < vSpecs.vSwapImageViews.size(); i++)
        {
            vkDestroyImageView(
                    vDeviceHandler->vSpecs.vDevice,
                    vSpecs.vSwapImageViews[i],
                    nullptr
                    );
        }

        vkDestroySwapchainKHR(
                vDeviceHandler->vSpecs.vDevice,
                vSpecs.vSwapChain,
                nullptr
                );
    }
}}

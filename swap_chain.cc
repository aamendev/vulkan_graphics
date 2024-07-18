#include "swap_chain.h"
namespace Lina{ namespace Graphics{
    b8 SwapChain::init(SwapChainSupportDetails* details, VkSurfaceKHR* surface,
            Window* window, DeviceHandler* d)
    {
        mSpecs.swapChainDetails = details;
        mSpecs.surface = surface;
        mWindow = window;
        mDeviceHandler = d;
        return
            createSwapChain()
            && createImageViews()
            && createRenderPass()
            && createDepthResources()
            && createFramebuffers()
            && createSyncObjects();
    }

    b8 SwapChain::createSwapChain()
    {
        mSpecs.format = chooseSwapSurfaceFormat();
        mSpecs.presentMode = chooseSwapPresentMode();
        mSpecs.extent = chooseSwapExtent();

        u32 imageCount = mSpecs.swapChainDetails->capabilities.minImageCount + 1;
        if (mSpecs.swapChainDetails->capabilities.maxImageCount > 0
                && imageCount > mSpecs.swapChainDetails->capabilities.maxImageCount)
        {
            imageCount = mSpecs.swapChainDetails->capabilities.maxImageCount;
        }

        u32 queueFamilyIndices[] =
        {
            (u32)mDeviceHandler->mSpecs.familyIndices.graphicsFamily,
            (u32)mDeviceHandler->mSpecs.familyIndices.presentFamily
        };

        VkSwapchainCreateInfoKHR createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = *(mSpecs.surface),
                .minImageCount = imageCount,
                .imageFormat = mSpecs.format.format,
                .imageColorSpace = mSpecs.format.colorSpace,
                .imageExtent = mSpecs.extent,
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,

                .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .queueFamilyIndexCount = 0,
                .pQueueFamilyIndices = nullptr,

                .preTransform = mSpecs.swapChainDetails->capabilities.currentTransform,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = mSpecs.presentMode,
                .clipped = VK_TRUE,

                .oldSwapchain = VK_NULL_HANDLE,
        };

        if (mDeviceHandler->mSpecs.familyIndices.graphicsFamily != mDeviceHandler->mSpecs.familyIndices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }

        vkCreateSwapchainKHR(mDeviceHandler->mSpecs.device, &createInfo, nullptr, &(mSpecs.swapChain));

        vkGetSwapchainImagesKHR(mDeviceHandler->mSpecs.device, mSpecs.swapChain, &imageCount, nullptr);
        mSpecs.swapImages.resize(imageCount);
        vkGetSwapchainImagesKHR(mDeviceHandler->mSpecs.device, mSpecs.swapChain, &imageCount, mSpecs.swapImages.data());

        return true;
    }

    b8 SwapChain::createImageViews()
    {
        mSpecs.swapImageViews.resize(mSpecs.swapImages.size());
        for (u32 i = 0; i < mSpecs.swapImages.size(); i++)
        {
            VkImageViewCreateInfo createInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .image = mSpecs.swapImages[i],
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = mSpecs.format.format,
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

            vkCreateImageView(mDeviceHandler->mSpecs.device, &createInfo, nullptr, &(mSpecs.swapImageViews[i]));
        }
        return true;
    }

    b8 SwapChain::createRenderPass()
    {
        VkSubpassDependency dependency
        {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
                .dstSubpass = 0,
                .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                .srcAccessMask = 0,
                .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
        };
        VkAttachmentDescription colorAttachment
        {
            .format = mSpecs.format.format,
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

        VkAttachmentDescription depthAttachment
        {
            .format = VK_FORMAT_D32_SFLOAT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };
        VkAttachmentReference depthAttachmentReference
        {
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass
        {
            .pipelineBindPoint=VK_PIPELINE_BIND_POINT_GRAPHICS,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachementReference,
            .pDepthStencilAttachment = &depthAttachmentReference
        };

        std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
        VkRenderPassCreateInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                .attachmentCount = attachments.size(),
                .pAttachments = attachments.data(),
                .subpassCount = 1,
                .pSubpasses = &subpass,
                .dependencyCount = 1,
                .pDependencies = &dependency,
        };
        return vkCreateRenderPass(
                mDeviceHandler->mSpecs.device,
                &renderPassInfo,
                nullptr,
                &(mSpecs.renderPass)) == VK_SUCCESS;
    }
    b8 SwapChain::createFramebuffers()
    {
        mSpecs.swapChainFrameBuffers.resize(mSpecs.swapImageViews.size());
        for (size_t i = 0; i < mSpecs.swapImageViews.size(); i++) {
            std::array<VkImageView,2> att = 
            {
                mSpecs.swapImageViews[i],
                mSpecs.depthImageView
            };

            VkFramebufferCreateInfo framebufferInfo =
            {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = mSpecs.renderPass,
            .attachmentCount = att.size(),
            .pAttachments = att.data(),
            .width = mSpecs.extent.width,
            .height = mSpecs.extent.height,
            .layers = 1,
            };

            if (vkCreateFramebuffer(
                        mDeviceHandler->mSpecs.device,
                        &framebufferInfo,
                        nullptr,
                        &(mSpecs.swapChainFrameBuffers[i])) != VK_SUCCESS) {
                return false;
            }
        }
        return true;
    }

    b8 SwapChain::createDepthResources()
    {
        VkImage* vDepthImage = new VkImage;
        mSpecs.depthImage.init
            (
             mDeviceHandler,
             vDepthImage,
             &mSpecs.depthImageMemory,
             mSpecs.extent.width,
             mSpecs.extent.height
            );

        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = *(mSpecs.depthImage.getImage());
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_D32_SFLOAT;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(mDeviceHandler->mSpecs.device, &viewInfo, nullptr, &mSpecs.depthImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
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
                mDeviceHandler->mSpecs.device,
                &semaphoreInfo,
                nullptr,
                &(mSpecs.imageAvailableSemaphore)) != VK_SUCCESS)
        {
            return false;
        }

        if(vkCreateSemaphore(
                mDeviceHandler->mSpecs.device,
                &semaphoreInfo,
                nullptr,
                &(mSpecs.renderFinishedSemaphore)) != VK_SUCCESS)
        {
            return false;
        }

        if(vkCreateFence(
                mDeviceHandler->mSpecs.device,
                &fenceInfo,
                nullptr,
                &(mSpecs.inFlightFence)) != VK_SUCCESS)
        {
            return false;
        }
        return true;
    }
    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat()
    {
        for (const auto& availableFormat : mSpecs.swapChainDetails->formats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
                    && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return mSpecs.swapChainDetails->formats[0];
    }
    VkPresentModeKHR SwapChain::chooseSwapPresentMode()
    {
        for (const auto& availablePresentMode : mSpecs.swapChainDetails->presnetModes)
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
        VkSurfaceCapabilitiesKHR cap = mSpecs.swapChainDetails->capabilities;
        /*if (cap.currentExtent.width != std::numeric_limits<u32>::max())
        {
            //return cap.currentExtent;
            std::cerr<< "BIG ISSUE\n";
        }*/
        i32 width, height;
        glfwGetFramebufferSize(mWindow->getWindow(), &width, &height);
        VkExtent2D actualExtent =
        {
            static_cast<u32>(width),
            static_cast<u32>(height)
        };
        //actualExtent.width = std::clamp(actualExtent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
        //actualExtent.height = std::clamp(actualExtent.height, cap.minImageExtent.height, cap.maxImageExtent.height);
        return actualExtent;
    }

    void SwapChain::recreate()
    {
        vkDeviceWaitIdle(mDeviceHandler->mSpecs.device);
        clean();
        createSwapChain();
        createImageViews();
        createDepthResources();
        createFramebuffers();
    }

    void SwapChain::clean()
    {
        vkDestroyImageView(mDeviceHandler->mSpecs.device,
                mSpecs.depthImageView,
                nullptr);
        vkDestroyImage(
                mDeviceHandler->mSpecs.device,
                *mSpecs.depthImage.getImage(),
                nullptr
                );
        vkFreeMemory(
                mDeviceHandler->mSpecs.device,
                mSpecs.depthImageMemory,
                nullptr
                );
        for (u32 i = 0; i < mSpecs.swapChainFrameBuffers.size(); i++)
        {
            vkDestroyFramebuffer(
                    mDeviceHandler->mSpecs.device,
                    mSpecs.swapChainFrameBuffers[i],
                    nullptr
                    );
        }

        for (u32 i = 0; i < mSpecs.swapImageViews.size(); i++)
        {
            vkDestroyImageView(
                    mDeviceHandler->mSpecs.device,
                    mSpecs.swapImageViews[i],
                    nullptr
                    );
        }
        vkDestroySwapchainKHR(
                mDeviceHandler->mSpecs.device,
                mSpecs.swapChain,
                nullptr
                );
    }
}}

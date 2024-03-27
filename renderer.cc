#include "renderer.h"
namespace Lina{ namespace Graphics{
    void Renderer::init(std::string& name, Window* window)
    {
        mSpecs =
        {
            .sWindow = window,
            .sVInitializer = new VulkanInitializer()
        };
        vShader = Shader();
        mSpecs.sVInitializer->init(name, window);
    }
    void Renderer::createVertexBuffer(VertexBufferLayout& layout,
            const std::vector<float>& vertices) {
        mSpecs.vBuffer.init(mSpecs.sVInitializer->vDeviceHandler, layout);
        mSpecs.vBuffer.constructFromDataPointer(&vertices[0],
                vertices.size() * sizeof(vertices[0]));
    }
    void Renderer::createIndexBuffer(const std::vector<u32> indices)
    {
        mSpecs.vIndexBuffer.init(mSpecs.sVInitializer->vDeviceHandler);
        mSpecs.vIndexBuffer.constructFromDataPointer(&indices[0],
                indices.size() * sizeof(indices[0]));
    }
    VkShaderModule Renderer::createShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                .codeSize = code.size(),
                .pCode = reinterpret_cast<const u32*>(code.data())
        };
        VkShaderModule module;
        vkCreateShaderModule(mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &createInfo, nullptr, &module);
        return module;
    }

    void Renderer::createDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .pImmutableSamplers = nullptr
        };
        VkDescriptorSetLayoutBinding samplerLayoutBinding
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .pImmutableSamplers = nullptr
        };

        std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = static_cast<u32>(bindings.size()),
            .pBindings = bindings.data()
        };
        if(vkCreateDescriptorSetLayout(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &layoutInfo,
                nullptr,
                &mSpecs.descriptorSetLayout) != VK_SUCCESS)
            std::cout<<"No layout created!\n";
    }

    void Renderer::createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 2> poolSizes{};

       poolSizes[0] =
       {
        .type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1
       };
       poolSizes[1] =
       {
        .type =  VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1
       };
        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 1,
            .poolSizeCount = static_cast<u32>(poolSizes.size()),
            .pPoolSizes = poolSizes.data(),
        };

        if(vkCreateDescriptorPool(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &poolInfo,
                nullptr,
                &mSpecs.descriptorPool) != VK_SUCCESS)
            std::cout<<"Error creating set\n";
    }

    void Renderer::createDescriptorSet()
    {
        std::vector<VkDescriptorSetLayout> layouts(1, mSpecs.descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = mSpecs.descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = layouts.data()
        };
        mSpecs.descriptorSets.reserve(1);
        if(vkAllocateDescriptorSets(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &allocInfo,
                mSpecs.descriptorSets.data()) != VK_SUCCESS)
            std::cout<<"Failed to allocate sets\n";
        VkDescriptorBufferInfo bufferInfo
        {
            .buffer = mSpecs.vUniformBuffer.mSpecs.buffer,
            .offset = 0,
            .range = mSpecs.vUniformBuffer.mSpecs.size
        };

        VkDescriptorImageInfo imageInfo
        {
            .sampler = mSpecs.textureSampler,
                .imageView = mSpecs.textureImageView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        std::array<VkWriteDescriptorSet, 2> descriptorWrite;
        descriptorWrite[0] =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mSpecs.descriptorSets[0],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        };
        descriptorWrite[1] =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mSpecs.descriptorSets[0],
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
        };

        vkUpdateDescriptorSets(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                static_cast<u32>(descriptorWrite.size()),
                descriptorWrite.data(),
                0,
                nullptr);
    }
    void Renderer::createUniformBuffers(u32 size)
    {
        mSpecs.vUniformBuffer.init(mSpecs.sVInitializer->vDeviceHandler);
        mSpecs.vUniformBuffer.constructFromUniformSize(size);
        createDescriptorPool();
        createDescriptorSet();
    }
    void Renderer::createGraphicsPipeline()
    {
        createDescriptorSetLayout();
        vShader.init("../shaders/shader.vert.spv", "../shaders/shader.frag.spv");
        mSpecs.vVertexShaderModule =  createShaderModule(vShader.getVertexShader());
        mSpecs.vFragmentShaderModule = createShaderModule(vShader.getFragmentShader());

        VkPipelineShaderStageCreateInfo vertShaderCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = mSpecs.vVertexShaderModule,
                .pName = "main"
        };
        VkPipelineShaderStageCreateInfo fragShaderCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = mSpecs.vFragmentShaderModule,
                .pName = "main"
        };

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderCreateInfo, fragShaderCreateInfo};
        /* Vertex Buffer */
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        auto attributeDescriptions = mSpecs.vBuffer.getAttributeDescriptions();
        auto bindingDescription = mSpecs.vBuffer.getBindingDescription();

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        /* End of Vertex Buffer */
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &mSpecs.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(
                    mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                    &pipelineLayoutInfo, nullptr, &(mSpecs.vPipelineLayout)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipeLineInfo
        {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .stageCount = 2,
                .pStages = shaderStages,

                .pVertexInputState = &vertexInputInfo,
                .pViewportState = &viewportState,
                .pRasterizationState = &rasterizer,
                .pMultisampleState = &multisampling,
                .pDepthStencilState = nullptr,
                .pColorBlendState = &colorBlending,
                .pDynamicState = &dynamicState,

                .layout = mSpecs.vPipelineLayout,
                .renderPass = mSpecs.sVInitializer->vSwapChain->vSpecs.vRenderPass,
                .subpass = 0,

                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = -1
        };

        vkCreateGraphicsPipelines(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                VK_NULL_HANDLE,
                1,
                &pipeLineInfo,
                nullptr,
                &(mSpecs.vGraphicsPipeline)
                );
        vkDestroyShaderModule(mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                mSpecs.vVertexShaderModule, nullptr);
        vkDestroyShaderModule(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                mSpecs.vFragmentShaderModule, nullptr);
    }
    void Renderer::beginRecordCommandBuffer(u32 imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = 0,
                .pInheritanceInfo = nullptr
        };
        vkBeginCommandBuffer(mSpecs.sVInitializer->vSpecs.vCommandBuffer, &beginInfo);
    }
    void Renderer::recordCommandBuffer(u32 imageIndex)
    {
        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = mSpecs.sVInitializer->vSwapChain->vSpecs.vRenderPass,
                .framebuffer = mSpecs.sVInitializer->vSwapChain->vSpecs.vSwapChainFrameBuffers[imageIndex],
                .renderArea = {.offset = {0, 0}, .extent = mSpecs.sVInitializer->vSwapChain->vSpecs.vExtent},
                .clearValueCount = 1,
                .pClearValues = &clearColor
        };

        vkCmdBeginRenderPass(
                mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(
                mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.vGraphicsPipeline);
        VkViewport viewport
        {
            .x = 0.0f,
                .y = 0.0f,
                .width = (f32)mSpecs.sVInitializer->vSwapChain->vSpecs.vExtent.width,
                .height = (f32)mSpecs.sVInitializer->vSwapChain->vSpecs.vExtent.height,
                .minDepth = 0.0f,
                .maxDepth = 100.0f
        };
        vkCmdSetViewport(mSpecs.sVInitializer->vSpecs.vCommandBuffer, 0, 1, &viewport);

        VkRect2D scissor
        {
            .offset = {0, 0},
                .extent = mSpecs.sVInitializer->vSwapChain->vSpecs.vExtent
        };

        vkCmdSetScissor(mSpecs.sVInitializer->vSpecs.vCommandBuffer, 0, 1, &scissor);
        vkCmdBindPipeline(mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.vGraphicsPipeline);

        VkBuffer vertexBuffers[] = {mSpecs.vBuffer.mSpecs.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(mSpecs.sVInitializer->vSpecs.vCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                mSpecs.vIndexBuffer.mSpecs.buffer, 0, VK_INDEX_TYPE_UINT32);
        auto vertexCount = mSpecs.vBuffer.getSize() / mSpecs.vBuffer.getStride();
       //vkCmdDraw(mSpecs.sVInitializer->vSpecs.vCommandBuffer,
         //       vertexCount , 1, 0, 0);
        vkCmdBindDescriptorSets(
                mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.vPipelineLayout,
                0,
                1,
                &mSpecs.descriptorSets[0],
                0,
                nullptr);
        vkCmdDrawIndexed(
                mSpecs.sVInitializer->vSpecs.vCommandBuffer,
                mSpecs.vIndexBuffer.mCount, 1, 0, 0, 0);
    }
    void Renderer::endRecordCommandBuffer(u32 image)
    {
        vkCmdEndRenderPass(mSpecs.sVInitializer->vSpecs.vCommandBuffer);
        vkEndCommandBuffer(mSpecs.sVInitializer->vSpecs.vCommandBuffer);
    }

    void Renderer::createTexture(std::string& path)
    {
        createTexture(std::move(path));
    }

    void Renderer::createTexture(std::string&& path)
    {
       mSpecs.texture.init(mSpecs.sVInitializer->vDeviceHandler);
       if(mSpecs.texture.createImageFromPath(path, false))
       {
       transitionImageLayout(
               mSpecs.texture.mImage,
               VK_FORMAT_R8G8B8A8_SRGB,
               VK_IMAGE_LAYOUT_UNDEFINED,
               VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
               );
       copyBufferToImage(mSpecs.texture.mStagingBuffer.mSpecs.buffer, mSpecs.texture.mTextureImage);
       transitionImageLayout(
                    mSpecs.texture.mImage,
                    VK_FORMAT_R8G8B8A8_SRGB,
                    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
               );
       createTextureImageView();
       createTextureSampler();
       }
    }

    void Renderer::beginDraw()
    {
        VulkanSpecs* vSpecs = mSpecs.sVInitializer->getVulkanSpecs();
        vkWaitForFences(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                1,
                &mSpecs.sVInitializer->vSwapChain->vSpecs.vInFlightFence,
                VK_TRUE,
                UINT64_MAX);

        uint32_t imageIndex;
        auto result = vkAcquireNextImageKHR(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                mSpecs.sVInitializer->vSwapChain->vSpecs.vSwapChain,
                UINT64_MAX,
                mSpecs.sVInitializer->vSwapChain->vSpecs.vImageAvailableSemaphore,
                VK_NULL_HANDLE,
                &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || mSpecs.sWindow->isResized())
        {
            mSpecs.sWindow->setResized(false);
            mSpecs.sVInitializer->vSwapChain->recreate();
            return;
        }

        vkResetFences(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                1,
                &(mSpecs.sVInitializer->vSwapChain->vSpecs.vInFlightFence)
                );

        vkResetCommandBuffer(vSpecs->vCommandBuffer, 0);


        beginRecordCommandBuffer(imageIndex);
        mHiddenDrawData.hImage = imageIndex;
    }
    void Renderer::endDraw()
    {
        VulkanSpecs* vSpecs = mSpecs.sVInitializer->getVulkanSpecs();
        recordCommandBuffer(mHiddenDrawData.hImage);
        endRecordCommandBuffer(mHiddenDrawData.hImage);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {mSpecs.sVInitializer->vSwapChain->vSpecs.vImageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vSpecs->vCommandBuffer;

        VkSemaphore signalSemaphores[] = {mSpecs.sVInitializer->vSwapChain->vSpecs.vRenderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit
                (mSpecs.sVInitializer->vDeviceHandler->vSpecs.vGraphicsQueue,
                 1,
                 &submitInfo,
                 mSpecs.sVInitializer->vSwapChain->vSpecs.vInFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {mSpecs.sVInitializer->vSwapChain->vSpecs.vSwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &mHiddenDrawData.hImage;

        vkQueuePresentKHR(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vPresentQueue,
                &presentInfo
                );


    }

    VkCommandBuffer Renderer::beginSingleTimeCommands()
    {

        VkCommandBufferAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = mSpecs.sVInitializer->vSpecs.vCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;

        vkAllocateCommandBuffers(mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }
    void Renderer::endSingleTimeCommands(VkCommandBuffer commandBuffer)
    {

        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer
        };

        vkQueueSubmit(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vGraphicsQueue,
                1, &submitInfo, VK_NULL_HANDLE);

        vkQueueWaitIdle(mSpecs.sVInitializer->vDeviceHandler->vSpecs.vGraphicsQueue);
        vkFreeCommandBuffers(mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                mSpecs.sVInitializer->vSpecs.vCommandPool, 1, &commandBuffer);
    }

    void Renderer::copyBufferToBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
    {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();
        VkBufferCopy copyRegion =
        {
            .size = size
        };
        vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

        endSingleTimeCommands(commandBuffer);
    }

    void Renderer::copyBufferToImage(VkBuffer src, Image& image)
    {
        VkCommandBuffer commandBuffer = beginSingleTimeCommands();


        VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,

            .imageSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .imageOffset = {0, 0, 0},
            .imageExtent = {image.getWidth(), image.getHeight(), 1}
        };

        vkCmdCopyBufferToImage(
                commandBuffer,
                src,
                image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
                );
        endSingleTimeCommands(commandBuffer);
    }

    void Renderer::transitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {
        bool case1 =
            (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_UNDEFINED);
        bool case2 =
            (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();


        VkPipelineStageFlags srcStage =
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT * case1 +
            VK_PIPELINE_STAGE_TRANSFER_BIT * case2;

        VkPipelineStageFlags dstStage =
            VK_PIPELINE_STAGE_TRANSFER_BIT * case1 +
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT * case2;

        u32 srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT * case2;

        u32 dstAccessMask  =
            VK_ACCESS_TRANSFER_WRITE_BIT * case1
        + VK_ACCESS_SHADER_READ_BIT * case2;

        VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };


        vkCmdPipelineBarrier(
                commandBuffer,
                srcStage,
                dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
                );
        endSingleTimeCommands(commandBuffer);
    }
    void Renderer::createTextureImageView()
    {
        VkImageViewCreateInfo imageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = mSpecs.texture.mImage,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_SRGB,
            .subresourceRange =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        vkCreateImageView
            (mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
             &imageCreateInfo, nullptr, &mSpecs.textureImageView
                );
    }

    void Renderer::createTextureSampler()
    {
        VkPhysicalDeviceProperties pdProperties;
        vkGetPhysicalDeviceProperties(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vPhysicalDevice,
                &pdProperties);
        VkSamplerCreateInfo samplerInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .mipLodBias = 0.0f,
            .anisotropyEnable = VK_TRUE,
            .maxAnisotropy = pdProperties.limits.maxSamplerAnisotropy,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .unnormalizedCoordinates = VK_FALSE,
        };
        vkCreateSampler(
                mSpecs.sVInitializer->vDeviceHandler->vSpecs.vDevice,
                &samplerInfo, nullptr, &mSpecs.textureSampler);
    }
    void Renderer::setPrimitive(Primitive p)
    {
        vkCmdSetPrimitiveTopology(mSpecs.sVInitializer->vSpecs.vCommandBuffer
                , static_cast<VkPrimitiveTopology>(p));
    }
}}

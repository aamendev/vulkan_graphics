#include "renderer.h"
#include "Math/Transform4D.h"
#include "index_buffer.h"
#include "vertex_buffer.h"
#include <vulkan/vulkan_core.h>
namespace Lina{ namespace Graphics{
    void Renderer::init(std::string& name, Window* window) {
        mSpecs = {.sWindow = window};
        mShader = Shader();
        mDeviceHandler = new DeviceHandler();
        mSwapChain = new SwapChain(); if (createVulkanInstance(name))
        {
            window->createWindowSurface(mSpecs.instance, &mSpecs.surface);

            mDeviceHandler->init(&mSpecs.instance, &mSpecs.surface);

            mSpecs.swapChainDetails = mDeviceHandler->mSpecs.swapChainDetails;

            mSwapChain->init(&mSpecs.swapChainDetails, &mSpecs.surface, window, mDeviceHandler);

            createCommandPool();
            createCommandBuffer();
        }
    }

    b8 Renderer::createVulkanInstance(std::string& name)
    {
        if (mSpecs.enableValidationLayers && !supportsValidationLayer())
        {
            std::cerr<<"Does not Support Validation\n";
        }
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
        if (mSpecs.enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<u32>(mSpecs.valdiationLayers.size());
            createInfo.ppEnabledLayerNames = mSpecs.valdiationLayers.data();
        }
        if (vkCreateInstance(&createInfo,nullptr, &(mSpecs.instance)) != VK_SUCCESS)
        {
            std::cerr<<"Failed to create Instance\n";
            return false;
        }
        return true;
    }

    void Renderer::createCommandPool()
    {
        VkCommandPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                .queueFamilyIndex = (u32)mDeviceHandler->mSpecs.familyIndices.graphicsFamily
        };
        vkCreateCommandPool(mDeviceHandler->mSpecs.device, &poolInfo, nullptr, &(mSpecs.commandPool));
    }

    void Renderer::createCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                .commandPool = mSpecs.commandPool,
                .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .commandBufferCount = 1
        };
        vkAllocateCommandBuffers(mDeviceHandler->mSpecs.device, &allocInfo, &(mSpecs.commandBuffer));
    }

    void Renderer::createVertexBuffer(VertexBufferLayout& layout,
            const std::vector<float>& vertices) {
        mSpecs.vertexBuffer.init(mDeviceHandler, layout);
        mSpecs.vertexBuffer.constructFromDataPointer(&vertices[0],
                vertices.size() * sizeof(vertices[0]));
    }
    void Renderer::createIndexBuffer(const std::vector<u32> indices)
    {
        mSpecs.indexBuffer.init(mDeviceHandler);
        mSpecs.indexBuffer.constructFromDataPointer(&indices[0],
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
        vkCreateShaderModule(mDeviceHandler->mSpecs.device,
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

        VkDescriptorSetLayoutBinding textureLayoutBinding
        {
            .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .descriptorCount = (u32)mSpecs.textures.size(),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
        };

        std::array<VkDescriptorSetLayoutBinding, 3> bindings = {uboLayoutBinding, samplerLayoutBinding, textureLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .bindingCount = static_cast<u32>(bindings.size()),
                .pBindings = bindings.data()
        };
        if(vkCreateDescriptorSetLayout(
                    mDeviceHandler->mSpecs.device,
                    &layoutInfo,
                    nullptr,
                    &mSpecs.descriptorSetLayout) != VK_SUCCESS)
            std::cerr<<"No layout created!\n";
    }

    void Renderer::createDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 3> poolSizes{};

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
        poolSizes[2] =
        {
            .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            .descriptorCount = (u32)mSpecs.textures.size()
        };
        VkDescriptorPoolCreateInfo poolInfo
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                .maxSets = 1,
                .poolSizeCount = static_cast<u32>(poolSizes.size()),
                .pPoolSizes = poolSizes.data(),
        };

        if(vkCreateDescriptorPool(
                    mDeviceHandler->mSpecs.device,
                    &poolInfo,
                    nullptr,
                    &mSpecs.descriptorPool) != VK_SUCCESS)
            std::cerr<<"Error creating set\n";
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
                    mDeviceHandler->mSpecs.device,
                    &allocInfo,
                    mSpecs.descriptorSets.data()) != VK_SUCCESS)
            std::cerr<<"Failed to allocate sets\n";
        VkDescriptorBufferInfo bufferInfo
        {
            .buffer = mSpecs.uniformBuffer.mSpecs.buffer,
                .offset = 0,
                .range = mSpecs.uniformBuffer.mSpecs.size
        };

        std::vector<VkWriteDescriptorSet> descriptorWrite;
        descriptorWrite.resize(3);
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
        std::vector<VkDescriptorImageInfo> imageInfos;
        imageInfos.resize(mSpecs.textures.size());
        for (int i = 0; i < imageInfos.size(); i++)
        {
             imageInfos[i] = 
            {
                .sampler = mSpecs.textureSampler,
                    .imageView = mSpecs.textureImageViews[i],
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };
        }
            descriptorWrite[1] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[0],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1, 
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfos[0],
            };

            descriptorWrite[2] =
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[0],
                .dstBinding = 2,
                .dstArrayElement = 0,
                .descriptorCount = (u32)mSpecs.textures.size(), 
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .pImageInfo = &imageInfos[0],
            };

        vkUpdateDescriptorSets(
                mDeviceHandler->mSpecs.device,
                static_cast<u32>(descriptorWrite.size()),
                descriptorWrite.data(),
                0,
                nullptr);
    }
    void Renderer::createUniformBuffers(u32 size)
    {
        mSpecs.uniformBuffer.init(mDeviceHandler);
        mSpecs.uniformBuffer.constructFromUniformSize(size);
        createDescriptorPool();
        createDescriptorSet();
    }

    void Renderer::createGraphicsPipeline()
    {
        createDescriptorSetLayout();
        mShader.init("../shaders/shader.vert.spv", "../shaders/shader.frag.spv");
        mSpecs.vertexShaderModule =  createShaderModule(mShader.getVertexShader());
        mSpecs.fragmentShaderModule = createShaderModule(mShader.getFragmentShader());

        VkPipelineShaderStageCreateInfo vertShaderCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = mSpecs.vertexShaderModule,
                .pName = "main"
        };
        VkPipelineShaderStageCreateInfo fragShaderCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = mSpecs.fragmentShaderModule,
                .pName = "main"
        };

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderCreateInfo, fragShaderCreateInfo};

        /* Vertex Buffer */
        auto attributeDescriptions = mSpecs.vertexBuffer.getAttributeDescriptions();
        auto bindingDescription = mSpecs.vertexBuffer.getBindingDescription();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .vertexBindingDescriptionCount = 1,
                .pVertexBindingDescriptions = &bindingDescription,
                .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
                .pVertexAttributeDescriptions = attributeDescriptions.data(),
        };
        /* End of Vertex Buffer */

        /* Depth and Stencil */
        VkPipelineDepthStencilStateCreateInfo depthStencil
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
                .depthTestEnable = VK_TRUE,
                .depthWriteEnable = VK_TRUE,
                .depthCompareOp = VK_COMPARE_OP_LESS,
                .depthBoundsTestEnable = VK_FALSE,
                .stencilTestEnable = VK_FALSE,
        };

        /* End Depth and Stencil*/
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
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment
        {
            .blendEnable = VK_FALSE,
                .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        VkPipelineColorBlendStateCreateInfo colorBlending =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .logicOpEnable = VK_FALSE,
            .logicOp = VK_LOGIC_OP_COPY,
            .attachmentCount = 1,
            .pAttachments = &colorBlendAttachment,
            .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f},
        };

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
            //VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPushConstantRange psRange =
        {
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
            .offset = 0,
            .size = sizeof(Lina::Math::Transform4D),
        };

        VkPushConstantRange psRangeTexture =
        {
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
            .offset = sizeof(Lina::Math::Transform4D),
            .size = sizeof(int),
        };
        std::array<VkPushConstantRange, 2> psRanges = {psRange, psRangeTexture};
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &mSpecs.descriptorSetLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 2;
        pipelineLayoutInfo.pPushConstantRanges = &psRanges[0];

        if (vkCreatePipelineLayout(
                    mDeviceHandler->mSpecs.device,
                    &pipelineLayoutInfo, nullptr, &(mSpecs.pipelineLayout)) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipeLineInfo
        {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .stageCount = 2,
            .pStages = shaderStages,

            .pVertexInputState = &vertexInputInfo,
            .pInputAssemblyState = &inputAssembly,
            .pViewportState = &viewportState,
            .pRasterizationState = &rasterizer,
            .pMultisampleState = &multisampling,
            .pDepthStencilState = &depthStencil,
            .pColorBlendState = &colorBlending,
            .pDynamicState = &dynamicState,

            .layout = mSpecs.pipelineLayout,
            .renderPass = mSwapChain->mSpecs.renderPass,
            .subpass = 0,

            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1
        };

        vkCreateGraphicsPipelines(
                mDeviceHandler->mSpecs.device,
                VK_NULL_HANDLE,
                1,
                &pipeLineInfo,
                nullptr,
                &(mSpecs.graphicsPipeline)
                );
        vkDestroyShaderModule(mDeviceHandler->mSpecs.device,
                mSpecs.vertexShaderModule, nullptr);
        vkDestroyShaderModule(
                mDeviceHandler->mSpecs.device,
                mSpecs.fragmentShaderModule, nullptr);
    }
    void Renderer::recordCommandBuffer()
    {
        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = 0,
                .pInheritanceInfo = nullptr
        };
        vkBeginCommandBuffer(mSpecs.commandBuffer, &beginInfo);
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = mSwapChain->mSpecs.renderPass,
                .framebuffer = mSwapChain->mSpecs.swapChainFrameBuffers[mHiddenDrawData.hImage],
                .renderArea = {.offset = {0, 0}, .extent = mSwapChain->mSpecs.extent},
                .clearValueCount = clearValues.size(),
                .pClearValues = clearValues.data() 
        };

        vkCmdBeginRenderPass(
                mSpecs.commandBuffer,
                &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.graphicsPipeline);
        VkViewport viewport
        {
            .x = 0.0f,
                .y = 0.0f,
                .width = (f32)mSwapChain->mSpecs.extent.width,
                .height = (f32)mSwapChain->mSpecs.extent.height,
                .minDepth = 0.00f,
                .maxDepth = 1.0f
        };
        vkCmdSetViewport(mSpecs.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor
        {
            .offset = {0, 0},
                .extent = mSwapChain->mSpecs.extent
        };

        vkCmdSetScissor(mSpecs.commandBuffer, 0, 1, &scissor);

        vkCmdBindDescriptorSets(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.pipelineLayout,
                0,
                1,
                &mSpecs.descriptorSets[0],
                0,
                nullptr);
    }
    void Renderer::render(VertexBuffer* vb, IndexBuffer* ib, int texId)
    {
        if (vb == nullptr)
            vb = &mSpecs.vertexBuffer;
        if (ib == nullptr)
            ib = &mSpecs.indexBuffer;
        VkBuffer vertexBuffers[] = {vb->mSpecs.buffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(mSpecs.commandBuffer, 0,  1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(mSpecs.commandBuffer,
                ib->mSpecs.buffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdPushConstants
            (
                mSpecs.commandBuffer,
                mSpecs.pipelineLayout,
                VK_SHADER_STAGE_FRAGMENT_BIT,
                64,
                sizeof(int),
                (void*)(&texId)
            );

        vkCmdDrawIndexed(
                mSpecs.commandBuffer,
                ib->mCount, 1, 0, 0, 0);
    }

    void Renderer::createTexture(std::string& path, b8 flip)
    {
        createTexture({{std::move(path), flip}});
    }

    void Renderer::createTexture(std::vector<std::pair<std::string, b8>> paths)
    {
        mSpecs.textures.resize(paths.size());
        mSpecs.textureImageViews.resize(paths.size());
        for (int i = 0; i < mSpecs.textures.size(); i++)
        {
            auto [path, flip] = paths[i];
            
            mSpecs.textures[i].init(mDeviceHandler);
            if(mSpecs.textures[i].createImageFromPath(path, flip))
            {
                transitionImageLayout(
                        mSpecs.textures[i].mImage,
                        VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_UNDEFINED,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                        );
                copyBufferToImage(mSpecs.textures[i].mStagingBuffer.mSpecs.buffer, mSpecs.textures[i].mTextureImage);
                transitionImageLayout(
                        mSpecs.textures[i].mImage,
                        VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
                        );
                createImageView(mSpecs.textures[i].mImage, VK_FORMAT_R8G8B8A8_SRGB,
                        VK_IMAGE_ASPECT_COLOR_BIT, i);
            }
        }
        createTextureSampler();
    }

    void Renderer::beginDraw()
    {
        vkWaitForFences(
                mDeviceHandler->mSpecs.device,
                1,
                &mSwapChain->mSpecs.inFlightFence,
                VK_TRUE,
                UINT64_MAX);

        uint32_t imageIndex;
        auto result = vkAcquireNextImageKHR(
                mDeviceHandler->mSpecs.device,
                mSwapChain->mSpecs.swapChain,
                UINT64_MAX,
                mSwapChain->mSpecs.imageAvailableSemaphore,
                VK_NULL_HANDLE,
                &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            mSwapChain->recreate();
            return;
        }
        mHiddenDrawData.hImage = imageIndex;
        recordCommandBuffer();
        //    vkResetCommandBuffer(mSpecs.commandBuffer, 0);
    }
    void Renderer::endDraw()
    {
        vkCmdEndRenderPass(mSpecs.commandBuffer);
        vkEndCommandBuffer(mSpecs.commandBuffer);
        vkResetFences(
                mDeviceHandler->mSpecs.device,
                1,
                &(mSwapChain->mSpecs.inFlightFence)
                );

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSemaphore waitSemaphores[] = {mSwapChain->mSpecs.imageAvailableSemaphore};
        VkSemaphore signalSemaphores[] = {mSwapChain->mSpecs.renderFinishedSemaphore};


        VkSubmitInfo submitInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,

            .commandBufferCount = 1,
            .pCommandBuffers = &mSpecs.commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        if (vkQueueSubmit
                (mDeviceHandler->mSpecs.graphicsQueue,
                 1,
                 &submitInfo,
                 mSwapChain->mSpecs.inFlightFence) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }
        VkSwapchainKHR swapChains[] = {mSwapChain->mSpecs.swapChain};

        VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,

            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,

            .swapchainCount = 1,
            .pSwapchains = swapChains,

            .pImageIndices = &mHiddenDrawData.hImage,
        };

        VkResult res = vkQueuePresentKHR(
                mDeviceHandler->mSpecs.presentQueue,
                &presentInfo
                );
        if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || mSpecs.sWindow->isResized())
        {
            mSpecs.sWindow->setResized(false);
            mSwapChain->recreate();
        }
    }

    VkCommandBuffer Renderer::beginSingleTimeCommands()
    {

        VkCommandBufferAllocateInfo allocInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = mSpecs.commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };

        VkCommandBuffer commandBuffer;

        vkAllocateCommandBuffers(mDeviceHandler->mSpecs.device,
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
                mDeviceHandler->mSpecs.graphicsQueue,
                1, &submitInfo, VK_NULL_HANDLE);

        vkQueueWaitIdle(mDeviceHandler->mSpecs.graphicsQueue);
        vkFreeCommandBuffers(mDeviceHandler->mSpecs.device,
                mSpecs.commandPool, 1, &commandBuffer);
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
                *image.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &region
                );
        endSingleTimeCommands(commandBuffer);
    }

    void Renderer::transitionImageLayout(VkImage& image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
    {

        bool depth = (format == VK_FORMAT_D32_SFLOAT);


        bool case3 =
            (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_UNDEFINED);

        bool case1 =
            (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_UNDEFINED);
        bool case2 =
            (newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL &&
             oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();


        VkPipelineStageFlags srcStage =
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT * (case1 || case3) +
            VK_PIPELINE_STAGE_TRANSFER_BIT * case2;

        VkPipelineStageFlags dstStage =
            VK_PIPELINE_STAGE_TRANSFER_BIT * case1 +
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT * case2
            + VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT * case3;

        u32 srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT * case2;

        u32 dstAccessMask  =
            VK_ACCESS_TRANSFER_WRITE_BIT * case1
            + VK_ACCESS_SHADER_READ_BIT * case2
            + (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT) * case3;

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

        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT * !depth +
            VK_IMAGE_ASPECT_DEPTH_BIT * depth;

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
    void Renderer::createImageView(
            VkImage& image,
            VkFormat format,
            VkImageAspectFlags aspFlags,
            int index)
    {
        VkImageViewCreateInfo imageCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .subresourceRange =
            {
                .aspectMask = aspFlags,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };
        vkCreateImageView
            (mDeviceHandler->mSpecs.device,
             &imageCreateInfo, nullptr, &mSpecs.textureImageViews[index]
            );
    }

    void Renderer::createTextureSampler()
    {
        VkPhysicalDeviceProperties pdProperties;
        vkGetPhysicalDeviceProperties(
                mDeviceHandler->mSpecs.physicalDevice,
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
                mDeviceHandler->mSpecs.device,
                &samplerInfo, nullptr, &mSpecs.textureSampler);
    }
    void Renderer::setPrimitive(Primitive p)
    {
        vkCmdSetPrimitiveTopology(mSpecs.commandBuffer
                , static_cast<VkPrimitiveTopology>(p));
    }

    void Renderer::enableDepthTest(bool val)
    {
        vkCmdSetDepthTestEnable(mSpecs.commandBuffer, static_cast<VkBool32>(val));
    }

    b8 Renderer::supportsValidationLayer()
    {
        u32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount,nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        for (const char* layerName : mSpecs.valdiationLayers)
        {
            bool layerFound = false;
            for (const auto& p : availableLayers)
            {
                std::cerr<<p.layerName<<'\n';
                if (strcmp(layerName, p.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) return false;
        }
        return true;
    }
    /*void VulkanInitializer::clean()
      {
      vkDestroySemaphore(vDeviceHandler->mSpecs.device, mSpecs.imageAvailableSemaphore, nullptr);
      vkDestroySemaphore(vDeviceHandler->mSpecs.device, mSpecs.renderFinishedSemaphore, nullptr);
      vkDestroyFence(vDeviceHandler->mSpecs.device, mSpecs.inFlightFence, nullptr);
      for (auto frameBuffer : mSpecs.swapChainFrameBuffers)
      {
      vkDestroyFramebuffer(vDeviceHandler->mSpecs.device, frameBuffer, nullptr);
      }
      vkDestroyPipeline(vDeviceHandler->mSpecs.device, mSpecs.graphicsPipeline, nullptr);
      vkDestroyPipelineLayout(vDeviceHandler->mSpecs.device, mSpecs.pipelineLayout, nullptr);
      vkDestroyRenderPass(vDeviceHandler->mSpecs.device, mSpecs.renderPass, nullptr);

      for (auto imageView : mSpecs.swapImageViews) {
      vkDestroyImageView(vDeviceHandler->mSpecs.device, imageView, nullptr);
      }

      vkDestroySwapchainKHR(vDeviceHandler->mSpecs.device, mSpecs.swapChain, nullptr);
      vkDestroyDevice(vDeviceHandler->mSpecs.device, nullptr);

      vkDestroySurfaceKHR(mSpecs.instance, mSpecs.surface, nullptr);
      vkDestroyInstance(mSpecs.instance, nullptr);


      }*/
}}

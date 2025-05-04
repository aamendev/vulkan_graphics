#include "renderer.h"
#include "Math/Transform4D.h"
#include "core/utils.h"
#include "index_buffer.h"
#include "types.h"
#include "vertex_buffer.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>
namespace Lina{ namespace Graphics{
    void Renderer::init(std::string& name, Window* window) {
        mSpecs = {.sWindow = window};
        mDeviceHandler = new DeviceHandler();
        mSwapChain = new SwapChain(); 
        mCurrentShader = 0;
        if (createVulkanInstance(name))
        {
            window->createWindowSurface(mSpecs.instance, &mSpecs.surface);

            mDeviceHandler->init(&mSpecs.instance, &mSpecs.surface);

            mSpecs.swapChainDetails = mDeviceHandler->mSpecs.swapChainDetails;

            mSwapChain->init(&mSpecs.swapChainDetails, &mSpecs.surface, window, mDeviceHandler);

            createCommandPool();
            createCommandBuffer();
        }
    }

    void Renderer::addShader(const Shader& shader)
    {
        mShaders.push_back(shader);
        mSpecs.vertexShaderModules.emplace_back();
        mSpecs.fragmentShaderModules.emplace_back();

        auto last = mShaders.size() - 1;
        mSpecs.vertexShaderModules[last] =
            createShaderModule(mShaders[last].getVertexShader());

        mSpecs.fragmentShaderModules[last] =
            createShaderModule(mShaders[last].getFragmentShader());
       /* if (mShaders[last].checkLoadMainTexture()) [[unlikely]]
        {
            mSpecs.mainTexture.init(mDeviceHandler);
            mSpecs.mainTexture.createMainTexture(getWidth(), getHeight());
            createTextureSampler();
            loadMainTexture();
        }*/
    }

    void Renderer::addShader(std::string&& vertexPath, std::string&& fragPath)
    {
        mShaders.emplace_back();
        mSpecs.vertexShaderModules.emplace_back();
        mSpecs.fragmentShaderModules.emplace_back();

        auto last = mShaders.size() - 1;
        mShaders[last].init(vertexPath, fragPath, "");
        mSpecs.vertexShaderModules[last] =
            createShaderModule(mShaders[last].getVertexShader());
        mSpecs.fragmentShaderModules[last] = 
            createShaderModule(mShaders[last].getFragmentShader());
        if (mShaders[last].checkLoadMainTexture()) [[unlikely]]
        {
            mSpecs.mainTexture.init(mDeviceHandler);
            mSpecs.mainTexture.createMainTexture(getWidth(), getHeight());
            createTextureSampler();
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
        VertexBuffer vb;
        vb.init(mDeviceHandler, layout);
        vb.constructFromDataPointer(&vertices[0],
                vertices.size() * sizeof(vertices[0]));
        mSpecs.vertexBuffer.push_back(vb);
    }
    void Renderer::createIndexBuffer(const std::vector<u32> indices)
    {
        IndexBuffer ib;
        ib.init(mDeviceHandler);
        ib.constructFromDataPointer(&indices[0],
                indices.size() * sizeof(indices[0]));
        mSpecs.indexBuffer.push_back(ib);
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

    void Renderer::createDescriptorSetLayout(int idx)
    {
        auto bindingsCount = mShaders[idx].getBindingSize();
        auto& uniformRef = mShaders[idx].getUniforms();
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bindings.reserve(bindingsCount + 2);

        VkDescriptorSetLayout setLayout;

        for (auto& ur : uniformRef)
        {
            VkDescriptorSetLayoutBinding uboLayoutBinding
            {
                .binding = ur.binding,
                    .descriptorType = Core::Utils::toVkBufferType(ur.type), 
                    .descriptorCount = 1,
                    .stageFlags = (VkShaderStageFlags)ur.stage, 
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(uboLayoutBinding);
        }

        if(mSpecs.textures.size() > 0)[[unlikely]]
        {
            VkDescriptorSetLayoutBinding samplerLayoutBinding
            {
                .binding = bindingsCount,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(samplerLayoutBinding);

            VkDescriptorSetLayoutBinding textureLayoutBinding
            {
                .binding = bindingsCount + 1,
                    .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                    .descriptorCount = (u32)mSpecs.textures.size(),
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(textureLayoutBinding);
        }
        if (mShaders[idx].checkLoadMainTexture())[[unlikely]]
        {
            VkDescriptorSetLayoutBinding colorAttachment
            {
                .binding = bindingsCount,
                    .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(colorAttachment);

            VkDescriptorSetLayoutBinding depthAttachment
            {
                .binding = bindingsCount + 1,
                    .descriptorType =VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 1,
                    .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .pImmutableSamplers = nullptr
            };
            bindings.push_back(depthAttachment);
        }

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
                    &setLayout) != VK_SUCCESS)
            std::cerr<<"No layout created!\n";
        mSpecs.descriptorSetLayout[idx] = setLayout;
    }

    void Renderer::createDescriptorPool(int idx)
    {

        auto bindingsCount = mShaders[idx].getBindingSize();
        auto& refUniforms = mShaders[idx].getUniforms();

        std::vector<VkDescriptorPoolSize> poolSizes;
        poolSizes.reserve(bindingsCount + 2);

        VkDescriptorPool vkPool;

        u32 u = 0;
        if ((u = mShaders[idx].getStaticUniformsCount()) > 0)
        {
        poolSizes.push_back(
                {
                .type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = u 
                });
        }

        if ((u = mShaders[idx].getDynamicUniformsCount()) > 0)
        {
        poolSizes.push_back(
                {
                .type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
                .descriptorCount = u 
                });
        }

        if (mSpecs.textures.size() > 0)[[unlikely]]
        {
            poolSizes.push_back(
                    {
                    .type =  VK_DESCRIPTOR_TYPE_SAMPLER,
                    .descriptorCount = 1
                    });
            poolSizes.push_back(
                    {
                    .type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                    .descriptorCount = (u32)mSpecs.textures.size()
                    });
        }
        if (mShaders[idx].checkLoadMainTexture())[[unlikely]]
        {
            poolSizes.push_back(
                    {
                    .type =  VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                    .descriptorCount = 2
                    });
        }
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
                    &vkPool) != VK_SUCCESS)
            std::cerr<<"Error creating set\n";
        mSpecs.descriptorPool[idx] = vkPool;
    }

    void Renderer::createDescriptorSet(int shaderIndex, int uniformStartIndex)
    {
        std::vector<VkDescriptorSetLayout> layouts(1, 
                mSpecs.descriptorSetLayout[shaderIndex]);

        VkDescriptorSetAllocateInfo allocInfo
        {
            .sType= VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                .descriptorPool = mSpecs.descriptorPool[shaderIndex],
                .descriptorSetCount = 1,
                .pSetLayouts = layouts.data()
        };
        if(vkAllocateDescriptorSets(
                    mDeviceHandler->mSpecs.device,
                    &allocInfo,
                    &mSpecs.descriptorSets[shaderIndex]) != VK_SUCCESS)
            std::cerr<<"Failed to allocate sets\n";

        std::vector<VkDescriptorImageInfo> imageInfos;
        imageInfos.resize(mSpecs.textures.size() + 
                2 * mShaders[shaderIndex].checkLoadMainTexture());

        std::vector<VkWriteDescriptorSet> descriptorWrite;
        auto bindingCount = mShaders[shaderIndex].getBindingSize();
        descriptorWrite.resize(bindingCount + 2 * (mSpecs.textures.size() > 0)
                + 2 * mShaders[shaderIndex].checkLoadMainTexture());

        std::vector<VkDescriptorBufferInfo> bufferInfos;
        bufferInfos.resize(bindingCount);

        auto& refUniform = mShaders[shaderIndex].getUniforms();

        for (int i = 0; i < refUniform.size(); i++)
        {
            bufferInfos[i] = 
            {
                .buffer = mSpecs.uniformBuffers[uniformStartIndex + i].mSpecs.buffer,
                .offset = 0,
                .range = mSpecs.uniformBuffers[uniformStartIndex + i].getSize(),
            };


            descriptorWrite[i] = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[shaderIndex],
                .dstBinding = refUniform[i].binding,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = Core::Utils::toVkBufferType(refUniform[i].type),
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfos[i],
                .pTexelBufferView = nullptr,
            };

        }

        if (mSpecs.textures.size() > 0)
        {
            for (int i = 0; i < imageInfos.size(); i++)
            {
                imageInfos[i] = 
                {
                    .sampler = mSpecs.textureSampler,
                    .imageView = mSpecs.textureImageViews[i],
                    .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                };
            }

            descriptorWrite[bindingCount] = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[shaderIndex],
                .dstBinding = bindingCount,
                .dstArrayElement = 0,
                .descriptorCount = 1, 
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                .pImageInfo = &imageInfos[0],
            };


            descriptorWrite[bindingCount + 1] = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[shaderIndex],
                .dstBinding = bindingCount + 1,
                .dstArrayElement = 0,
                .descriptorCount = (u32)mSpecs.textures.size(), 
                .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
                .pImageInfo = &imageInfos[0],
            };
        }

        if (mShaders[shaderIndex].checkLoadMainTexture())
        {
            auto last = imageInfos.size() - 2*!imageInfos.empty();
            std::cerr << last << '\n';
            imageInfos[last] = 
            {
                .sampler = VK_NULL_HANDLE,
                .imageView = mSwapChain->mSpecs.colImageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };
            imageInfos[last + 1] = 
            {
                .sampler = VK_NULL_HANDLE,
                .imageView = mSwapChain->mSpecs.depthImageView,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

            descriptorWrite[bindingCount] = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[shaderIndex],
                .dstBinding = bindingCount,
                .dstArrayElement = 0,
                .descriptorCount = 1, 
                .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .pImageInfo = &imageInfos[last],
            };

            descriptorWrite[bindingCount + 1] = 
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .dstSet = mSpecs.descriptorSets[shaderIndex],
                .dstBinding = bindingCount + 1,
                .dstArrayElement = 0,
                .descriptorCount = 1, 
                .descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
                .pImageInfo = &imageInfos[last + 1],
            };
        }

        vkUpdateDescriptorSets(
                mDeviceHandler->mSpecs.device,
                static_cast<u32>(descriptorWrite.size()),
                descriptorWrite.data(),
                0,
                nullptr);
    }
    void Renderer::createUniformBuffers()
    {
        for (int i = 0; i < mShaders.size(); i++)
        {
            mShaders[i].combineUniforms();
            auto& refUniform = mShaders[i].getUniforms();
            for (int j = 0; j < refUniform.size(); j++)
            {
                mSpecs.uniformBuffers.emplace_back();
                auto last = mSpecs.uniformBuffers.size() - 1;
                mSpecs.uniformBuffers[last].init(mDeviceHandler);
                mSpecs.uniformBuffers[last]
                    .constructFromUniformSize(refUniform[j].size, refUniform[j].count);
            }
        }
    }

    void Renderer::createGraphicsPipelines()
    {
        mSpecs.graphicsPipelines.resize(mShaderDataMap.size()); mSpecs.pipelineLayouts.resize(mShaderDataMap.size()); mSpecs.descriptorSetLayout.resize(mShaders.size());
        mSpecs.descriptorSets.resize(mShaders.size());
        mSpecs.descriptorPool.resize(mShaders.size());
        createUniformBuffers();

        auto uniformStart = 0;
        for (int i = 0; i < mShaders.size(); i++)
        {
            uniformStart += ((i > 0) * mShaders[i - 1].getBindingSize());
            createDescriptorSetLayout(i);
            createDescriptorPool(i);
            createDescriptorSet(i, uniformStart);
        }
        for (int i = 0; i < mShaderDataMap.size(); i++)
        {
            auto [shaderId, vertexDataId] = mShaderDataMap[i];
            VkPipelineShaderStageCreateInfo vertShaderCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = mSpecs.vertexShaderModules[shaderId],
                    .pName = "main"
            };

            VkPipelineShaderStageCreateInfo fragShaderCreateInfo
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = mSpecs.fragmentShaderModules[shaderId],
                    .pName = "main"
            };


            VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderCreateInfo, fragShaderCreateInfo};

            /* Vertex Buffer */
            auto attributeDescriptions = mSpecs.vertexBuffer[vertexDataId].getAttributeDescriptions();
            auto bindingDescription = mSpecs.vertexBuffer[vertexDataId].getBindingDescription();

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
           // inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;


            VkPipelineViewportStateCreateInfo viewportState{};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.scissorCount = 1;

            VkPipelineRasterizationStateCreateInfo rasterizer{};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_TRUE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_NONE;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;

            VkPipelineMultisampleStateCreateInfo multisampling{};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

            VkPipelineColorBlendAttachmentState colorBlendAttachment
            {
                .blendEnable = VK_TRUE,
                    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
                    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
                    .colorBlendOp = VK_BLEND_OP_ADD,
                    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                    .alphaBlendOp = VK_BLEND_OP_ADD,
                    .colorWriteMask = 
                        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT 
                        | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
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

            std::vector<VkPushConstantRange> psRanges;
            psRanges.reserve(mShaders[shaderId].getPushConstantSize());
            auto& refPs = mShaders[shaderId].getPushConstants();
            for (auto& rps : refPs)
            {
                VkPushConstantRange psRange =
                {
                    .stageFlags = (VkShaderStageFlags)rps.stage,
                    .offset = rps.offset,
                    .size = rps.size,
                };
                psRanges.push_back(psRange);
            }

            VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = 1;
            pipelineLayoutInfo.pSetLayouts = &mSpecs.descriptorSetLayout[shaderId];
            pipelineLayoutInfo.pushConstantRangeCount = psRanges.size();
            pipelineLayoutInfo.pPushConstantRanges = &psRanges[0];

            if (vkCreatePipelineLayout(
                        mDeviceHandler->mSpecs.device,
                        &pipelineLayoutInfo, nullptr, &(mSpecs.pipelineLayouts[i])) != VK_SUCCESS) {
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

                .layout = mSpecs.pipelineLayouts[i],
                .renderPass = mSwapChain->mSpecs.renderPass,
                .subpass = mShaders[shaderId].checkLoadMainTexture(),

                .basePipelineHandle = VK_NULL_HANDLE,
                .basePipelineIndex = -1,
            };

            vkCreateGraphicsPipelines(
                    mDeviceHandler->mSpecs.device,
                    VK_NULL_HANDLE,
                    1,
                    &pipeLineInfo,
                    nullptr,
                    &(mSpecs.graphicsPipelines[i])
                    );
        }
        for (int i = 0; i < mShaders.size(); i++)
        {
        vkDestroyShaderModule(mDeviceHandler->mSpecs.device,
                mSpecs.vertexShaderModules[i], nullptr);
        vkDestroyShaderModule(
                mDeviceHandler->mSpecs.device,
                mSpecs.fragmentShaderModules[i], nullptr);
        }
    }

    void Renderer::recordCommandBuffer()
    {
        std::array<VkClearValue, 3> clearValues = {};
        clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[1].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
        clearValues[2].depthStencil = {1.0f, 0};
        VkRenderPassBeginInfo renderPassInfo
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .renderPass = mSwapChain->mSpecs.renderPass,
                .framebuffer = mSwapChain->mSpecs.swapChainFrameBuffers[mHiddenDrawData.hImage],
                .renderArea = {.offset = {0, 0}, .extent = mSwapChain->mSpecs.extent},
                .clearValueCount = clearValues.size(),
                .pClearValues = clearValues.data(),
        };

        vkCmdBeginRenderPass(
                mSpecs.commandBuffer,
                &renderPassInfo,
                VK_SUBPASS_CONTENTS_INLINE);

    }
    void Renderer::bindShader(int idx, int subpass)
    {
        mCurrentDataMap = idx;
        mCurrentShader = mShaderDataMap[idx].first;
        bindPipeline(subpass);
    }

    void Renderer::bindPipeline(int subpass)
    {
        /*vkCmdBindDescriptorSets(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.pipelineLayouts[mCurrentShader],
                0,
                1,
                &mSpecs.descriptorSets[mCurrentShader],
                0,
                nullptr);*/

        vkCmdBindPipeline(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.graphicsPipelines[mCurrentDataMap + subpass]);
    }

    void Renderer::render()
    {
        auto vertId = mShaderDataMap[mCurrentDataMap].second;
        auto vb = &mSpecs.vertexBuffer[vertId];
        auto ib = &mSpecs.indexBuffer[vertId];
        VkBuffer vertexBuffers[] = {vb->mSpecs.buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(mSpecs.commandBuffer, 0,  1, vertexBuffers, offsets);

        vkCmdBindIndexBuffer(mSpecs.commandBuffer,
                ib->mSpecs.buffer, 0, VK_INDEX_TYPE_UINT32);

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

    void Renderer::loadMainTexture()
    {
        //mSpecs.mainTexture.createMainTexture(getWidth(), getHeight());
        auto& img = mSpecs.mainTexture.mImage;
        auto& currImg = mSwapChain->mSpecs.swapImages[mHiddenDrawData.hImage];

        transitionImageLayout(currImg, 
                VK_IMAGE_LAYOUT_UNDEFINED, 
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_ACCESS_MEMORY_READ_BIT, 
                VK_ACCESS_TRANSFER_READ_BIT);

        transitionImageLayout(img, 
                VK_IMAGE_LAYOUT_UNDEFINED, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                (VkAccessFlagBits)0, 
                VK_ACCESS_TRANSFER_WRITE_BIT);

        copyImageToImage(currImg, img);

        transitionImageLayout(
                img, 
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_ACCESS_SHADER_READ_BIT
                );

        transitionImageLayout(currImg, 
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_PIPELINE_STAGE_TRANSFER_BIT, 
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_ACCESS_MEMORY_READ_BIT 
                );

        createMainImageView(img, VK_FORMAT_R8G8B8A8_SRGB,
                VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void Renderer::copyImageToImage(VkImage src, VkImage& dst)
    {
        auto bcBuffer = beginSingleTimeCommands();
        VkImageCopy copyRegion = 
        {
            .srcSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .srcOffset = {0, 0, 0},
            .dstSubresource =
            {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1
            },
            .dstOffset = {0, 0, 0},
            .extent = {(u32)getWidth(), (u32)getHeight(), 1}
        };
        
        vkCmdCopyImage(
                bcBuffer,
                src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1,
                &copyRegion);
        endSingleTimeCommands(bcBuffer);
    }

    void Renderer::beginDraw()
    {
    }
    void Renderer::endDraw()
    {
    }
    void Renderer::beginPasses()
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

        VkCommandBufferBeginInfo beginInfo
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .flags = 0,
                .pInheritanceInfo = nullptr
        };
        vkBeginCommandBuffer(mSpecs.commandBuffer, &beginInfo);
        VkViewport viewport
        {
            .x = 0.0f,
                .y = 0.0f,
                .width = (f32)mSwapChain->mSpecs.extent.width,
                .height = (f32)mSwapChain->mSpecs.extent.height,
                .minDepth = 0.01f,
                .maxDepth = 1.0f
        };
        vkCmdSetViewport(mSpecs.commandBuffer, 0, 1, &viewport);

        VkRect2D scissor
        {
            .offset = {0, 0},
                .extent = mSwapChain->mSpecs.extent
        };

        vkCmdSetScissor(mSpecs.commandBuffer, 0, 1, &scissor);
        //    vkResetCommandBuffer(mSpecs.commandBuffer, 0);
        recordCommandBuffer();
    }

    void Renderer::nextPass()
    {
        //static int next = 1;
        //bindShader(mCurrentShader, next);
       // next++;
        vkCmdNextSubpass(mSpecs.commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
    }
    void Renderer::endPass()
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
    }
    void Renderer::present()
    {
        //VkSemaphore signalSemaphores[] = {mSwapChain->mSpecs.renderFinishedSemaphore};


        VkSwapchainKHR swapChains[] = {mSwapChain->mSpecs.swapChain};

        VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,

          //  .waitSemaphoreCount = 1,
         //   .pWaitSemaphores = signalSemaphores,

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

    void Renderer::copyBufferToBuffer(VkBuffer& src, VkBuffer& dst, VkDeviceSize size)
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

        void Renderer::transitionImageLayout(VkImage& image, 
          VkImageLayout oldLayout, 
                VkImageLayout newLayout,
                VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage,
                VkAccessFlagBits srcAccessMask, VkAccessFlagBits dstAccessMask)
        {

        VkCommandBuffer commandBuffer = beginSingleTimeCommands();

        VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, 
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
    void Renderer::transitionImageLayout(VkImage& image, 
            VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
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

        b8 caseScreenShot = 
            (newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && 
            (oldLayout == VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED, .image = image,
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

    void Renderer::createMainImageView(
            VkImage& image,
            VkFormat format,
            VkImageAspectFlags aspFlags)
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
             &imageCreateInfo, nullptr, &mSpecs.mainTextureImageView 
            );
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
        auto val = vkCreateSampler(
                mDeviceHandler->mSpecs.device,
                &samplerInfo, nullptr, &mSpecs.textureSampler);
    }
    void Renderer::setPrimitive(Primitive p)
    { vkCmdSetPrimitiveTopology(mSpecs.commandBuffer , static_cast<VkPrimitiveTopology>(p));
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

    void Renderer::updateUniform(void* data, u32 uniformId, u32 idx)
    {
        auto currIndex = 0;
        auto& ref = mShaders[mCurrentShader].getUniforms();
        for (int i = 0; i < mCurrentShader; i++) 
        {currIndex += mShaders[i].getBindingSize();}
        mSpecs.uniformBuffers[currIndex + uniformId].updateUniform(data, idx);

       /* if (ref[uniformId].type == UniformType::Dynamic)
        {
            dynOffset = idx * mSpecs.uniformBuffers[currIndex + uniformId].getSize();
            dynPointer = &dynOffset;
            dynCount = 1;
        }
        else 
        {
            dynPointer = nullptr;
        }
        std::cerr << "OFFSET: "<< dynOffset << '\n';
        vkCmdBindDescriptorSets(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.pipelineLayouts[mCurrentDataMap],
                0,
                1,
                &mSpecs.descriptorSets[mCurrentShader],
                dynCount,
                dynPointer);*/

    }
    void Renderer::submitUniformUpdates()
    {
        auto dynCount = mShaders[mCurrentShader].getDynamicUniformsCount();
        auto& dynRefs = mShaders[mCurrentShader].getDynamicUniforms();
        int currIndex = 0;
        for (int i = 0; i < mCurrentShader; i++) 
        {currIndex += mShaders[i].getBindingSize();}

        std::vector<u32> offsets;
        offsets.reserve(dynCount);

        for (int i = 0; i < dynCount; i++)
        {
            u32 dynOffset = mSpecs.uniformBuffers[currIndex + 
                mShaders[mCurrentShader].getStaticUniformsCount() + i].getSize();
            for (int j = 0; j < dynRefs[i].count; j++)
            {
                offsets.push_back(j * dynOffset);
            }
        }
        dynCount = offsets.size();

        u32* dynPointer = (dynCount == 0) ? nullptr : offsets.data();

        vkCmdBindDescriptorSets(
                mSpecs.commandBuffer,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mSpecs.pipelineLayouts[mCurrentDataMap],
                0,
                1,
                &mSpecs.descriptorSets[mCurrentShader],
                dynCount,
                dynPointer);
    }

    void Renderer::updatePushConstant(void* data, u32 pushConstantId)
    {
        auto& ps = mShaders[mCurrentShader].getPushConstants();
        vkCmdPushConstants(
                mSpecs.commandBuffer,
                mSpecs.pipelineLayouts[mCurrentDataMap],
                (VkPipelineStageFlags)ps[pushConstantId].stage,
                ps[pushConstantId].offset,
                ps[pushConstantId].size,
                data);
    }

    /*void VulkanInitializer::clean()

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

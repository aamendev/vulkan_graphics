#pragma once
#include "common.h"
#include "window.h"
#include "index_buffer.h"
#include "uniform_buffer.h"
#include "./vertex_buffer.h"
#include "image.h"
#include "swap_chain.h"
#include "shader.h"
#include "./depth_image.h"
#include "texture.h"
#include "./Math/Point3D.h"
#include "./Math/Transform4D.h"
namespace Lina{ namespace Graphics{

    class Renderer
    {
        struct RenderSpecs
        {
            VkInstance vInstance;

            VkSurfaceKHR vSurface;

            SwapChainSupportDetails vSwapChainDetails;

            VkCommandPool vCommandPool;
            VkCommandBuffer vCommandBuffer;

            Window* sWindow;
            VertexBuffer vBuffer;
            IndexBuffer vIndexBuffer;

            Texture texture;
            VkImageView textureImageView;
            VkSampler textureSampler;

            DepthImage depthImage;
            VkDeviceMemory depthImageMemory;
            VkImageView depthImageView;

            UniformBuffer vUniformBuffer;
            VkDeviceMemory uniformBuffersMemory;
            void* uniformBuffersMapped;

            VkDescriptorSetLayout descriptorSetLayout;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            VkPipelineLayout vPipelineLayout;
            VkPipeline vGraphicsPipeline;

            VkShaderModule vVertexShaderModule;
            VkShaderModule vFragmentShaderModule;
        };
        struct HiddenDrawData
        {
            u32 hImage;
        };

        public:
        void init(std::string& name, Window* window);
        RenderSpecs* getSpecs() {return &mSpecs;}

        void setPrimitive(Primitive p);
        void beginDraw();
        void endDraw();
        void createVertexBuffer(
                VertexBufferLayout& layout,
                const std::vector<float>& data);
        void createIndexBuffer(const std::vector<u32> indices);
        void createUniformBuffers(u32 size);
        void createGraphicsPipeline();
        void createTexture(std::string& path);
        void createTexture(std::string&& path);
        void updateUniform(void* data)
        {
            mSpecs.vUniformBuffer.updateUniform(data);
        }

        void createDepthResources();

        private:
        void beginRecordCommandBuffer(u32);
        void recordCommandBuffer(u32);
        void endRecordCommandBuffer(u32);
        VkShaderModule createShaderModule(const std::vector<char>& code);

        void createDescriptorSetLayout();
        void createDescriptorPool();
        void createDescriptorSet();

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer buffer);

        void copyBufferToBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
        void copyBufferToImage(VkBuffer src, Image& image);

        void transitionImageLayout(VkImage&, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        void createImageView(VkImage&, VkFormat, VkImageAspectFlags);
        void createTextureSampler();
        void createCommandBuffer();
        void createCommandPool();
        b8 createVulkanInstance(std::string& appName);
        void createWindowSurface();
        private:
        RenderSpecs mSpecs;
        Shader vShader;
        HiddenDrawData mHiddenDrawData;
        DeviceHandler* mDeviceHandler;
        SwapChain* mSwapChain;
    };
}}

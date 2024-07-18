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
#include <vulkan/vulkan_core.h>
namespace Lina{ namespace Graphics{

    class Renderer
    {
        struct RenderSpecs
        {

            std::vector<const char*> valdiationLayers = {"VK_LAYER_KHRONOS_validation"};
            bool enableValidationLayers = true;
            VkInstance instance;

            VkSurfaceKHR surface;

            SwapChainSupportDetails swapChainDetails;

            VkCommandPool commandPool;
            VkCommandBuffer commandBuffer;

            Window* sWindow;
            VertexBuffer vertexBuffer;
            IndexBuffer indexBuffer;

            Texture texture;
            VkImageView textureImageView;
            VkSampler textureSampler;


            UniformBuffer uniformBuffer;
            VkDeviceMemory uniformBuffersMemory;
            void* uniformBuffersMapped;

            VkDescriptorSetLayout descriptorSetLayout;
            VkDescriptorPool descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            VkPipelineLayout pipelineLayout;
            VkPipeline graphicsPipeline;

            VkShaderModule vertexShaderModule;
            VkShaderModule fragmentShaderModule;
        };
        struct HiddenDrawData
        {
            u32 hImage;
        };

        public:
        void init(std::string& name, Window* window);
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
            mSpecs.uniformBuffer.updateUniform(data);
            vkCmdPushConstants(
                    mSpecs.commandBuffer,
                    mSpecs.pipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(Math::Transform4D),
                    data);
        }
        
        void pushConstants(u32 size);
        void render(VertexBuffer* vb = nullptr, IndexBuffer* ib = nullptr);
        // Options //
        void setPrimitive(Primitive p);
        void enableDepthTest(bool);

        // GET //
        RenderSpecs* getSpecs() {return &mSpecs;}
        private:
        void createDepthResources();
        void recordCommandBuffer();
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

        b8 supportsValidationLayer();
        private:
        RenderSpecs mSpecs;
        Shader mShader;
        HiddenDrawData mHiddenDrawData;
        DeviceHandler* mDeviceHandler;
        SwapChain* mSwapChain;
        bool mSwapChainRecreated;
    };
}}

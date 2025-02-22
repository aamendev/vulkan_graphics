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
#include <utility>
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
            std::vector<VertexBuffer> vertexBuffer;
            std::vector<IndexBuffer> indexBuffer;

            std::vector<Texture> textures;
            std::vector<VkImageView> textureImageViews;
            VkSampler textureSampler;

            std::vector<UniformBuffer> uniformBuffers;
            VkDeviceMemory uniformBuffersMemory;
            void* uniformBuffersMapped;

            std::vector<VkDescriptorSetLayout> descriptorSetLayout;
            std::vector<VkDescriptorPool> descriptorPool;
            std::vector<VkDescriptorSet> descriptorSets;

            std::vector<VkPipelineLayout> pipelineLayouts;
            std::vector<VkPipeline> graphicsPipelines;

            std::vector<VkShaderModule> vertexShaderModules;
            std::vector<VkShaderModule> fragmentShaderModules;
        };
        struct HiddenDrawData
        {
            u32 hImage;
        };

        public:
        void init(std::string& name, Window* window);
        void beginDraw();
        void endDraw();
        void bindShader(int idx); 
        void bindPipeline(int idx);
        void addShader(std::string&&, std::string&&);
        void createVertexBuffer(
                VertexBufferLayout& layout,
                const std::vector<float>& data);
        void createIndexBuffer(const std::vector<u32> indices);
        void createUniformBuffers(u32 size);
        void createGraphicsPipelines();
        void createTexture(std::string& path, b8 flip);
        void createTexture(std::vector<std::pair<std::string, b8>> paths);
        void updateUniform(void* data, int idx)
        {
            mSpecs.uniformBuffers[idx].updateUniform(data);
            vkCmdPushConstants(
                    mSpecs.commandBuffer,
                    mSpecs.pipelineLayouts[currentShader],
                    VK_SHADER_STAGE_VERTEX_BIT * (currentShader == 0) +
                    VK_SHADER_STAGE_FRAGMENT_BIT * !(currentShader == 0),
                    0,
                    mSpecs.uniformBuffers[idx].getSize(),
                    data);
        }
        
        void pushConstants(u32 size);
        void render(VertexBuffer* vb = nullptr, IndexBuffer* ib = nullptr, int texId = 0);
        // Options //
        void setPrimitive(Primitive p);
        void enableDepthTest(bool);

        // GET //
        RenderSpecs* getSpecs() {return &mSpecs;}
        void toggleValidation() {mSpecs.enableValidationLayers = !mSpecs.enableValidationLayers;};
        private:
        void createDepthResources();
        void recordCommandBuffer();
        VkShaderModule createShaderModule(const std::vector<char>& code);

        void createDescriptorSetLayout(int idx);
        void createDescriptorPool(int idx);
        void createDescriptorSet(int idx);

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer buffer);

        void copyBufferToBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
        void copyBufferToImage(VkBuffer src, Image& image);

        void transitionImageLayout(VkImage&, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

        void createImageView(VkImage&, VkFormat, VkImageAspectFlags, int);
        void createTextureSampler();
        void createCommandBuffer();
        void createCommandPool();
        b8 createVulkanInstance(std::string& appName);
        void createWindowSurface();

        b8 supportsValidationLayer();
        private:
        RenderSpecs mSpecs;
        std::vector<Shader> mShaders;
        HiddenDrawData mHiddenDrawData;
        DeviceHandler* mDeviceHandler;
        SwapChain* mSwapChain;
        bool mSwapChainRecreated;

        int currentShader;
    };
}}

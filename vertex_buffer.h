#pragma once
#include "common.h"
#include "./devices_handler.h"
#include <cstring>
#include "./vertex_buffer_layout.h"
#include "./buffer.h"
namespace Lina{ namespace Graphics{
    class VertexBuffer: public Buffer
    {
        friend class Renderer;
        friend class VulkanInitializer;
        public:
        void init(DeviceHandler* device, VertexBufferLayout& layout);
        void constructFromDataPointer(
                const void* data,
                u32 size
                );

        /* Getters */
        inline VkVertexInputBindingDescription getBindingDescription()
        {
            return mInputBindingDescription;
        }

        inline std::vector<VkVertexInputAttributeDescription>
            getAttributeDescriptions()
            {
                return mAttributeDescriptions;
            }
        inline u32 getStride()
        {
            return mLayout.getStride();
        }
        /* End Getters */
        private:
        VertexBufferLayout mLayout;

        std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;
        VkVertexInputBindingDescription mInputBindingDescription;
    };
}}

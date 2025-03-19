#pragma once
#include "common.h"
#include "./devices_handler.h"
#include <cstring>
#include "./vertex_buffer_layout.h"
#include "./buffer.h"
namespace Lina{ namespace Graphics{
    class UniformBuffer: public Buffer
    {
        friend class Renderer;
        friend class VulkanInitializer;
        public:
        void init(DeviceHandler* device);
        void constructFromUniformSize(u32 size, u32 count = 1);

        void updateUniform(void* data, u32 id = 0);

        private:
            void* mDataPointer;
    };
}}

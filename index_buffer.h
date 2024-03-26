#pragma once
#include "common.h"
#include "./devices_handler.h"
#include <cstring>
#include "./vertex_buffer_layout.h"
#include "./buffer.h"
namespace Lina{ namespace Graphics{
    class IndexBuffer: public Buffer
    {
        friend class Renderer;
        friend class VulkanInitializer;
        public:
        void init(DeviceHandler* device);
        void constructFromDataPointer(
                const void* data,
                u32 size
                );
        u32 getCount() const {return mCount;}
        private:
        u32 mCount;
    };
}}

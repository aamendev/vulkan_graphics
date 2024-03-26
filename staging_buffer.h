#pragma once
#include "./common.h"
#include "./buffer.h"
#include <cstring>
namespace Lina{ namespace Graphics{
    class StagingBuffer : public Buffer
    {
        public:
            friend class Texture;
            friend class Renderer;
            b8 init(DeviceHandler* deviceHandler);
            b8 mapMemory(u32 size);
            b8 copyMemory(void* dist);
        private:
            void* mDataPointer;
    };
}}

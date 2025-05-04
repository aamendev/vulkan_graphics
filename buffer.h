#pragma once
#include "common.h"
#include "./devices_handler.h"
#include "./utils.h"
namespace Lina{ namespace Graphics{
    class Buffer
    {
        struct specs
        {
            u32 count = 1;
            VkDeviceSize size;
            u32 unAlignedSize = 0;
            VkBufferUsageFlags usage;
            VkMemoryPropertyFlags properties;
            VkBuffer buffer;
            VkDeviceMemory bufferMemory;
        };
        public:
            virtual void createBuffer();
            inline VkDeviceSize getSize() const{ return mSpecs.size; }
            inline VkBuffer& getBuffer() { return mSpecs.buffer; }
            inline u32 getCount() const {return mSpecs.count;}
        protected:
            DeviceHandler* mDeviceHandler;
            specs mSpecs;

    };
}}

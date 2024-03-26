#pragma once
#include "common.h"
#include "./devices_handler.h"
#include "./utils.h"
namespace Lina{ namespace Graphics{
    class Buffer
    {
        struct specs
        {
            VkDeviceSize size;
            VkBufferUsageFlags usage;
            VkMemoryPropertyFlags properties;
            VkBuffer buffer;
            VkDeviceMemory bufferMemory;
        };
        public:
            virtual void createBuffer();
            VkDeviceSize getSize() const{ return mSpecs.size; }
            VkBuffer getBuffer() const { return mSpecs.buffer; }
        protected:
            DeviceHandler* mDeviceHandler;
            specs mSpecs;

    };
}}

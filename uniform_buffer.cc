#include "./uniform_buffer.h"
#include <vulkan/vulkan_core.h>

namespace Lina{ namespace Graphics{
    void UniformBuffer::init(DeviceHandler* deviceHandler)
    {
        mDeviceHandler = deviceHandler;
        mSpecs.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
           // | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    void UniformBuffer::constructFromUniformSize(u32 size, u32 count)
        {
            mSpecs.unAlignedSize = size;
            auto minAlign = mDeviceHandler->mSpecs.limits.minUniformBufferOffsetAlignment;
            mSpecs.size = size;
            if (minAlign > 0)
            {
            mSpecs.size = (size + minAlign - 1) & ~(minAlign - 1);
            }
            mSpecs.count = count;
            createBuffer();

            vkMapMemory(
                    mDeviceHandler->mSpecs.device,
                    mSpecs.bufferMemory,
                    0,
                    mSpecs.size * count,
                    0,
                    &mDataPointer);
        }

    void UniformBuffer::updateUniform(void *data, u32 id)
    {
        std::memcpy((void*)((u8*)mDataPointer + id * mSpecs.size), data, 
                mSpecs.size);

        VkMappedMemoryRange flushMem
        {
            .sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            .memory = mSpecs.bufferMemory,
            .offset = id * mSpecs.size,
            .size = mSpecs.size
        };
        
       vkFlushMappedMemoryRanges(mDeviceHandler->mSpecs.device, 
                1,
                &flushMem);
                
    }
}}

#include "./staging_buffer.h"

namespace Lina{ namespace Graphics{
    b8 StagingBuffer::init(DeviceHandler* deviceHandler)
    {
        mDeviceHandler = deviceHandler;
        mSpecs.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        return true;
    }
    b8 StagingBuffer::mapMemory(u32 size)
    {
        mSpecs.size = size;
        createBuffer();

        vkMapMemory(
                mDeviceHandler->mSpecs.device,
                mSpecs.bufferMemory,
                0,
                size,
                0,
                &mDataPointer
                );
        return true;
    }
    b8 StagingBuffer::copyMemory(void* dist)
    {
        std::memcpy(mDataPointer, dist, mSpecs.size);
        vkUnmapMemory(mDeviceHandler->mSpecs.device, mSpecs.bufferMemory);
        return true;
    }
}}

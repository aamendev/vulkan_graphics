#include "./index_buffer.h"

namespace Lina{ namespace Graphics{
    void IndexBuffer::init(DeviceHandler* deviceHandler)
    {
        mCount = 0;
        mDeviceHandler = deviceHandler;
        mSpecs.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    void IndexBuffer::constructFromDataPointer(const void *data, u32 size)
        {
            mSpecs.size = size;
            mCount = size / 4;
            createBuffer();

            void* bindAddress;
            vkMapMemory(
                    mDeviceHandler->mSpecs.device,
                    mSpecs.bufferMemory,
                    0,
                    size,
                    0,
                    &bindAddress);
            std::memcpy(bindAddress, data, size);
            vkUnmapMemory(mDeviceHandler->mSpecs.device, mSpecs.bufferMemory);
        }
}}

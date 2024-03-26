#include "./uniform_buffer.h"

namespace Lina{ namespace Graphics{
    void UniformBuffer::init(DeviceHandler* deviceHandler)
    {
        mDeviceHandler = deviceHandler;
        mSpecs.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    void UniformBuffer::constructFromUniformSize(u32 size)
        {
            mSpecs.size = size;
            createBuffer();

            vkMapMemory(
                    mDeviceHandler->vSpecs.vDevice,
                    mSpecs.bufferMemory,
                    0,
                    size,
                    0,
                    &mDataPointer);
        }
    void UniformBuffer::updateUniform(void *data)
    {
        std::memcpy(mDataPointer, data, mSpecs.size);
        f32* newData = (float*)mDataPointer;
    }
}}

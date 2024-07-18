#include "./vertex_buffer.h"

namespace Lina{ namespace Graphics{
    void VertexBuffer::init(DeviceHandler* deviceHandler, VertexBufferLayout& layout)
    {
        mDeviceHandler = deviceHandler;
        mLayout = layout;
        mSpecs.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    void VertexBuffer::constructFromDataPointer(const void *data, u32 size)
        {
            mSpecs.size = size;
            createBuffer();

            mInputBindingDescription = {};
            mInputBindingDescription.binding = 0;
            mInputBindingDescription.stride = mLayout.getStride();
            mInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


            auto elements = mLayout.getElements();
            mAttributeDescriptions.resize(elements.size());
            u32 offset = 0;
            for (u32 i = 0; i < mAttributeDescriptions.size(); i++)
            {
                mAttributeDescriptions[i] = {};
                mAttributeDescriptions[i].binding = 0;
                mAttributeDescriptions[i].location = elements[i].location;
                mAttributeDescriptions[i].format = static_cast<VkFormat>(elements[i].format);
                mAttributeDescriptions[i].offset = offset;
                offset +=
                    elements[i].getCountOfFormat(elements[i].format)
                    * elements[i].getSizeOfFormat(elements[i].format);
            }

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

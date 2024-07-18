#include "./buffer.h"

namespace Lina{ namespace Graphics{
    void Buffer::createBuffer()
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = mSpecs.size;
        bufferInfo.usage = mSpecs.usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(mDeviceHandler->mSpecs.device, &bufferInfo, nullptr, &mSpecs.buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(mDeviceHandler->mSpecs.device, mSpecs.buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex =
            Utils::findMemoryType(mDeviceHandler->mSpecs.physicalDevice,
                        memRequirements.memoryTypeBits,
                        mSpecs.properties
                        );

        auto allocResult = (vkAllocateMemory(mDeviceHandler->mSpecs.device,
                    &allocInfo,
                    nullptr,
                    &mSpecs.bufferMemory));
        if  (allocResult != VK_SUCCESS){
            std::cout<<"RESULT: " << allocResult<< '\n';
            throw std::runtime_error("failed to allocate buffer memory!");
        }

        vkBindBufferMemory(mDeviceHandler->mSpecs.device, mSpecs.buffer, mSpecs.bufferMemory, 0);
    }
}}

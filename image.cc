#include "./image.h"

namespace Lina{ namespace Graphics{
    void Image::createImage()
    {
        VkImageCreateInfo imageInfo
            {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                    .flags = 0,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = mSpecs.format,
                .extent =
                    {
                        .width = mSpecs.width,
                        .height = mSpecs.height,
                        .depth = 1
                    },
                .mipLevels = 1,
                .arrayLayers = 1,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = mSpecs.tiling,
                .usage = mSpecs.usage,
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            };

            vkCreateImage(
                    mDeviceHandler->vSpecs.vDevice,
                    &imageInfo,
                    nullptr,
                    &mSpecs.image
                    );
            VkMemoryRequirements memRequirements;
            vkGetImageMemoryRequirements(
                    mDeviceHandler->vSpecs.vDevice,
                    mSpecs.image,
                    &memRequirements
                    );
            VkMemoryAllocateInfo memAllocInfo =
            {
                .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                .allocationSize = memRequirements.size,
                .memoryTypeIndex = Utils::findMemoryType(
                        mDeviceHandler->vSpecs.vPhysicalDevice,
                        memRequirements.memoryTypeBits,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
                        )
            };
            vkAllocateMemory(
                    mDeviceHandler->vSpecs.vDevice,
                    &memAllocInfo,
                    nullptr,
                    &mSpecs.imageMemory
                    );
            vkBindImageMemory(
                    mDeviceHandler->vSpecs.vDevice,
                    mSpecs.image,
                    mSpecs.imageMemory,
                    0
                    );
    }

    const VkImage& Image::getImage() const
    {
        return mSpecs.image;
    }

    u32 Image::getWidth() const
    {
        return mSpecs.width;
    }

    u32 Image::getHeight() const
    {
        return mSpecs.height;
    }

    const ImageSpecs& Image::getSpecs() const
    {
        return mSpecs;
    }
}}

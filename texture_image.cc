#include "./texture_image.h"

namespace Lina{ namespace Graphics{
    void TextureImage::init(DeviceHandler* deviceHandler,VkImage& image, VkDeviceMemory& memory, i32 width, i32 height)
    {
        mDeviceHandler = deviceHandler;
        mSpecs.width = width;
        mSpecs.height = height;
        mSpecs.format = VK_FORMAT_R8G8B8A8_SRGB;
        mSpecs.tiling = VK_IMAGE_TILING_OPTIMAL;
        mSpecs.usage = 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        mSpecs.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        mSpecs.image = &image;
        mSpecs.imageMemory = &memory;
        createImage();
    };
}}

#include "./color_image.h"
#include <vulkan/vulkan_core.h>

namespace Lina{ namespace Graphics{
   void ColorImage::init(
           DeviceHandler* deviceHandler,
           VkImage* image,
           VkDeviceMemory* memory,
           i32 width,
           i32 height)
   {
        mDeviceHandler = deviceHandler;
        mSpecs.width = width;
        mSpecs.height = height;
        mSpecs.format = mColorFormat;
        mSpecs.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        mSpecs.tiling = VK_IMAGE_TILING_OPTIMAL;
        mSpecs.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        mSpecs.image = image;
        mSpecs.imageMemory = memory;
        createImage();
   }
}}

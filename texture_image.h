#pragma once
#include "./common.h"
#include "./image.h"
#include "./utils.h"
namespace Lina{ namespace Graphics{
    class TextureImage : public Image
    {
        public:
            //void init(u32 width, u32 height, VkImage& image, VkDeviceMemory& memory);
       // void init(DeviceHandler* deviceHandler,i32 width, i32 height);
        void init(DeviceHandler* deviceHandler,VkImage& image, VkDeviceMemory& memory, i32 width, i32 height);
    };
}}

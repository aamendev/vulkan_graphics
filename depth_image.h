#pragma once
#include "./common.h"
#include "./image.h"

namespace Lina{ namespace Graphics{
    class DepthImage : public Image
    {
        public:
       void init(DeviceHandler* deviceHandler, VkImage& image, VkDeviceMemory& memory, i32 width, i32 height);
    };
}}

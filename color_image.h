#ifndef COLOR_IMAGE_H
#define COLOR_IMAGE_H
#include "./common.h"
#include "./image.h"
#include <vulkan/vulkan_core.h>

namespace Lina{ namespace Graphics{
    class ColorImage : public Image
    {
        public:
       void init(DeviceHandler* deviceHandler, VkImage* image, VkDeviceMemory* memory, i32 width, i32 height);
       public:
        inline void setFormat(VkFormat v) {mColorFormat = v;}

        private:
            VkFormat mColorFormat;
    };
}}
#endif

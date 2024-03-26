#pragma once
#include "./common.h"
#include "./devices_handler.h"
#include "./utils.h"
namespace Lina{ namespace Graphics{
    struct ImageSpecs
    {
        u32 width;
        u32 height;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        VkImage image;
        VkDeviceMemory imageMemory;
    };
    class Image
    {
        public:
            virtual void createImage();

            /*Getters */
            const VkImage& getImage() const;
            u32 getWidth() const;
            u32 getHeight() const;
            const ImageSpecs& getSpecs() const;
        protected:
            DeviceHandler* mDeviceHandler;
            ImageSpecs mSpecs;
    };
}}

#pragma once
#include "./common.h"
#include "./devices_handler.h"
#include "./utils.h"
namespace Lina{ namespace Graphics{
    struct ImageSpecs
    {
        u32 width;
        u32 height;
        u32 depth = 1;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkMemoryPropertyFlags properties;
        VkImage* image;
        VkDeviceMemory* imageMemory;
    };
    class Image
    {
        public:
            virtual void createImage();

            /*Getters */
            VkImage* getImage() const;
            u32 getWidth() const;
            u32 getHeight() const;
            inline u32 getDepth() const {return mSpecs.depth;}
            const ImageSpecs& getSpecs() const;
        protected:
            DeviceHandler* mDeviceHandler;
            ImageSpecs mSpecs;
    };
}}

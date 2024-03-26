#include "./texture.h"

namespace Lina { namespace Graphics{

    b8 Texture::init(DeviceHandler* deviceHandler)
    {
       mDeviceHandler = deviceHandler;
       return true;
    }

    b8 Texture::createImageFromPath(const std::string& path, bool flip)
    {
        return setTextureFromPath(path, flip);
    }
    b8 Texture::createImageFromPath(std::string&& path, bool flip)
    {
        return setTextureFromPath(std::move(path), flip);
    }

    b8 Texture::setTextureFromPath(const std::string& path, bool flip)
    {
        i32 width, height, channels;
        stbi_set_flip_vertically_on_load(flip);
        stbi_uc* data = nullptr;
        {
           data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        }
        if (data)
        {
            mWidth = width;
            mHeight = height;
            mStagingBuffer.copyMemory(data);
            stbi_image_free(data);
            mTextureImage.init(mDeviceHandler, mImage, mImageMemory, width, height);
            return true;
        }
        return false;
    }
}}

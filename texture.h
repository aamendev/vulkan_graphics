#include "common.h"
#include "./third_party/stb_image.h"
#include "./staging_buffer.h"
#include "./texture_image.h"
#include "core/noise_generator.h"
namespace Lina { namespace Graphics{
    class Texture
    {
        public:
            friend class Renderer;
            b8 init(DeviceHandler* deviceHandler);
            b8 createImageFromPath(std::string&& path, bool flip = false);
            b8 createImageFromPath(std::string& path, bool flip = false);
            b8 createImageFromNoise(Core::NoiseGenerator& ng);
            b8 createMainTexture(u32 width, u32 height);
            void recreate(u32 width, u32 height);
        public:
            inline std::string getPath() const {return mPath;}
            inline VkBuffer& getStagingBuffer() {return mStagingBuffer.getBuffer();}
        private:
            b8 setTextureFromPath(std::string&& path, bool flip);
        private:
            std::string mPath;
            i32 mWidth;
            i32 mHeight;
            i32 mDepth;
            StagingBuffer mStagingBuffer;
            DeviceHandler* mDeviceHandler;
            TextureImage mTextureImage;
            VkImage mImage;
            VkDeviceMemory mImageMemory;
    };
}}

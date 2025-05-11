#include "noise_generator.h"
namespace Lina { namespace Core {
    void NoiseGenerator::normalize()
    {
        f32 min = FLT_MAX;
        f32 max = -FLT_MAX;
        f32 current;
        for (int i = 0; i < mData.size(); i++)
        {
            current = mData[i];
            min = min * (min < current) + current * !(min < current);
            max = max * (max > current) + current * !(max > current);
        }
        for (int i = 0; i < mData.size(); i++)
        {
            // (x - min) / (max-min) = y/(1-0)
            mData[i] = (mData[i] - min) / (max - min);
        }
    }
    void NoiseGenerator::generateTexture()
    {
        fbm(0.8f, 1.2f, 12);
        normalize();
    }
    void NoiseGenerator::fbm(f32 h, f32 lac, f32 octaves)
    {
        if (!mInit)[[unlikely]]
            init_fbm(h, lac);
        Math::Point3D currentPoint;
        f32 currentOut;
        mData.clear();
        mData.resize(mDepth * mWidth * mHeight);
        for (int z = 0; z < mDepth; z++)
        {
            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    currentPoint = {(f32)x / mWidth, (f32)y / mHeight, (f32)z / mDepth};
                    currentOut = perPointFbm(currentPoint, h, lac, octaves); 
                    mData[z * mHeight * mWidth + y * mWidth + x] = 
                        (currentOut * mMulFactor);
                }
            }
        }
    }
}}

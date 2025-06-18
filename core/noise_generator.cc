#include "noise_generator.h"
#include "utils.h"
#include <algorithm>
#include <cmath>
namespace Lina { namespace Core {
    void NoiseGenerator::zeroData()
    {
        for (auto& d : mData)
        {
            d = Utils::randFloat(0, 1);
        }
    }
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
            mData[i] = 1 - ((mData[i] - min) / (max - min));
        }
    }
    void NoiseGenerator::initWorley(int randPoints)
    {
        mPoints.clear();
        mPoints.resize(randPoints);
        for (int i = 0; i < randPoints; i++)
        {

            mPoints[i] =  
            {
                (f32)(rand() % mWidth), 
                (f32)(rand() % mHeight), 
                (f32)(rand() % mDepth), 
            };
        }
    }
    void NoiseGenerator::worley(int n)
    {
        Math::Point3D currentPoint;
        f32 currentOut;
        mData.clear();
        mData.resize(mDepth * mWidth * mHeight);
        std::vector<f32> distances(mPoints.size());
        for (int z = 0; z < mDepth; z++)
        {
            for (int y = 0; y < mHeight; y++)
            {
                for (int x = 0; x < mWidth; x++)
                {
                    currentPoint = (Math::Point3D){(f32)x, (f32)y, (f32)z}
                    + (Math::Point3D){
                        Utils::randFloat(0, 1),
                        Utils::randFloat(0, 1),
                        Utils::randFloat(0, 1),
                    };

                    for (int i = 0; i < mPoints.size(); i++)
                    {
                        distances[i] = 
                            mPoints[i].distance(currentPoint);
                    }
                    std::sort(distances.begin(), distances.end());
                    f32 val = distances[n]; 
                    //currentOut = perPointWarp(currentPoint, h, lac, octaves, dist); 
                    mData[z * mHeight * mWidth + y * mWidth + x] = 
                        (val * mMulFactor);
                }
            }
        }
    }
    void NoiseGenerator::generateTexture()
    {
      // fbm(0.8f, 1.2f, 12);
        mMulFactor = 1.0f;
      //  warp(0.8f, 1.2f, 12, 0.7f); 
       initWorley(mWidth * mHeight * mDepth / 1e3);
       worley(0);
      //mData.resize(mWidth * mHeight * mDepth);
      //zeroData();
    normalize();
    }

    void NoiseGenerator::warp(f32 h, f32 lac, f32 octaves, f32 dist)
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
                    currentPoint = (Math::Point3D){(f32)x, (f32)y, (f32)z}
                    + (Math::Point3D){
                        Utils::randFloat(0, 1),
                        Utils::randFloat(0, 1),
                        Utils::randFloat(0, 1),
                    };
                    currentOut = perPointWarp(currentPoint, h, lac, octaves, dist); 
                    mData[z * mHeight * mWidth + y * mWidth + x] = 
                        (currentOut * mMulFactor);
                }
            }
        }
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

#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H
#include "../modifier.h"
namespace Lina { namespace Core {
    class NoiseGenerator : public Graphics::Modifier
    {
        public:
        NoiseGenerator(u32 w = 512, u32 h = 512, u32 d = 1): 
            mWidth(w), mHeight(h), mDepth(d), mData({}) {init(); 
                mMulFactor = 1.0f;}

        public:
            void generateTexture();
            virtual void fbm(f32 h, f32 lac, f32 octaves) override;
            void warp(f32 h, f32 lac, f32 octaves, f32 dist);
            void zeroData();
            void worley(int n);
            void initWorley(int n);
        public:
            inline void setWidth(u32 f) {mWidth = f;}
            inline void setHeight(u32 f) {mHeight = f;}
            inline void setDepth(u32 f) {mDepth = f;}
        public:
            inline u32 getWidth() const {return mWidth;}
            inline u32 getHeight() const {return mHeight;}
            inline u32 getDepth() const {return mDepth;}
            inline const std::vector<f32>& getData() const {return mData;}
            inline u32 getPoint(u32 idx) const {return mData[idx];}
            inline u32 getPoint(u32 idxh, u32 idxv) const 
            {return mData[idxv * mWidth + idxh];}
            inline u32 getPoint(u32 x, u32 y, u32 z) const 
            {return mData[z * mWidth * mHeight + y * mWidth + z];}
        private:
            void normalize();
        private:
            std::vector<f32> mData;
            std::vector<Math::Point3D> mPoints;
            u32 mWidth;
            u32 mHeight;
            u32 mDepth;
    };
}}
#endif

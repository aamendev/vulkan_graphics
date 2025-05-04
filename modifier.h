#ifndef MODIFIER_H
#define MODIFIER_H
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "types.h"
#include "core/utils.h"
#define MAX_OCTAVES (64)
namespace Lina{ namespace Graphics{
    class Modifier 
    {
        public:
            virtual void init();
            virtual void fbm(f32 h, f32 lac, f32 octaves) = 0;
           // virtual void hybridMultiFractal(f32 h, f32 lac, f32 octaves, f32 offset) = 0;
            //virtual void warp(f32 h, f32 lac, f32 octaves, f32 dist) = 0;
        public:
            const std::vector<f32>& getVertices() {return mVertices;}
        public:
            inline void setVertices(std::vector<f32>& verts) {mVertices = verts;}
        public:
            inline void setMulFactor(f32 f) {mMulFactor = f;}
            inline void setThreshold(f32 f) {mTheshold = f;}

        protected:
            void initGradientTable();
            f32 glattice(int ix, int iy, int iz, f32 fx, f32 fy, f32 fz);
            f32 gnoise(f32 x, f32 y, f32 z);
            void init_fbm(float H, float lac);
            f32 perPointFbm(Math::Point3D p, f32 h, f32 lac, f32 octaves);
            f32 perPointHybridMultiFractal(Math::Point3D p, 
                    f32 h, f32 lac, f32 octaves, f32 offset);
            f32 perPointWarp(Math::Point3D p, f32 h, f32 lac, f32 octaves, f32 dist);
        protected:
            std::vector<f32> mVertices = {};
            std::vector<f32> mGradientTab = {}; 
            std::array<f32, MAX_OCTAVES> mExponentTab;
            f32 mMulFactor = 1.0f;
            b8 mInit = false;
            f32 mTheshold = 1.0f;
    };
}}
#endif

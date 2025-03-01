#ifndef CYLINDER_H
#define CYLINDER_H
#include "../Math/math.h"
#include "renderable.h"
#include <vector>
#include <array>
#include "../types.h"
namespace Lina{ namespace Graphics{ namespace Shapes{
    class Cylinder : public Renderable{
    public:
        Cylinder() {}
        Cylinder(float radius, f32 height, u32 sectorCount, 
                MeshMode m = MeshMode::Pos3Col3Tex2,
                Material mat = {.col= {1.0f, 1.0f, 1.0f}});
        void constructCylinder(f32 radius, f32 height, u32 sectorCount);
        void setRadius(float radius) {mRadius = radius;}
        void setSectors(u32 sectorCount) {mSectorCount = sectorCount;};
        float getRadius() const {return mRadius;}
    private:
        float mRadius;
        u32 mSectorCount;
        u32 mHeight;
    };
}}}
#endif

#pragma once
#include "../Math/math.h"
#include "renderable.h"
#include <vector>
#include <array>
namespace Lina{ namespace Graphics{ namespace Shapes{
    class Icosphere : public Renderable{
    public:
        Icosphere() {}
        Icosphere(float radius);
        void constructSphere(float radius);
        void setRadius(float radius) {mRadius = radius;}
        void subdivide(unsigned int layers);
        float getRadius() const {return mRadius;}
    private:
        float mRadius;
        int subdivisions;
        void addVertices(float v1, float v2, float v3);
        void addIndices();
        void computeMidPoint(Math::Point3D& v1,
                             Math::Point3D& v2, Math::Point3D& newV);
        void computeMidPointUV(uv& t0, uv& t1,
                                uv& newT);
        std::vector<int> getWinded();
        void fixSeam();
        void fixPoles();
    };
}}}


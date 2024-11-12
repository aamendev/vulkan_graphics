#pragma once
#include "../Math/math.h"
#include <vector>
namespace Lina{ namespace Graphics{
    struct uv
    {
        float u, v;
    };
    class Renderable
    {
        public:
            std::vector<Math::Point3D> getVertices() const {return mVertices;}
            std::vector<unsigned int> getIndices() const {return mIndices;}
            std::vector<uv> getTextureCoordinates() const {return mTextureCoordinates;}
            std::vector<float> getFullVertices() const {return mFullVertices;}
            std::vector<float> getTextureDebug() const;
            std::vector<float> getDebugVertices() const {return mDebugVertices;}
        protected:
            std::vector<Math::Point3D> mVertices;
            std::vector<unsigned int> mIndices;
            std::vector<uv> mTextureCoordinates;
            std::vector<float> mFullVertices;
            std::vector<float> mDebugVertices;
        protected:
            void computeFullVertices();

    };
}}


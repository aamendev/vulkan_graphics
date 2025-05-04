#pragma once
#include "../Math/math.h"
#include <vector>
#include "../types.h"
namespace Lina{ namespace Graphics{
    struct uv
    {
        float u, v;
    };
    struct Material
    {
        Math::Vector3D col;
    };
    class Renderable
    {
        public:
            std::vector<Math::Point3D> getVertices() const {return mVertices;}
            std::vector<unsigned int> getIndices() const {return mIndices;}
            std::vector<Math::Vector3D> getNormals() const {return mNormals;}
            std::vector<uv> getTextureCoordinates() const {return mTextureCoordinates;}
            std::vector<float> getFullVertices() const {return mFullVertices;}
            std::vector<float> getTextureDebug() const;
            std::vector<float> getDebugVertices() const {return mDebugVertices;}
        public:
            inline void setMeshMode(MeshMode m) {mMeshMode = m;}
        protected:
            std::vector<Math::Point3D> mVertices;
            std::vector<unsigned int> mIndices;
            std::vector<uv> mTextureCoordinates;
            std::vector<Math::Vector3D> mNormals;
            std::vector<float> mFullVertices;
            std::vector<float> mDebugVertices;
            MeshMode mMeshMode = MeshMode::Pos3Col3Tex2;
            Material mMaterial = {.col = {1.0f, 1.0f, 1.0f}};
        protected:
            void computeFullVertices();

    };
}}


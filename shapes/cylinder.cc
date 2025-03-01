#include "cylinder.h"
#include <cmath>
namespace Lina{ namespace Graphics{ namespace Shapes{
    Cylinder::Cylinder(float radius, f32 height, u32 sectorCount,
            MeshMode m, Material mat)
        {
            mMeshMode = m;
            mMaterial = mat;
            mRadius = radius;
            mSectorCount = sectorCount;
            mHeight = height;
            constructCylinder(radius, height, sectorCount);
        }

        void Cylinder::constructCylinder(f32 radius, f32 height, u32 sectorCount)
        {
            mVertices.clear();
            mFullVertices.clear();

            float h = 0.0f;
            float sectorStep = 2 * PI / mSectorCount;   
            float sectorAngle = 0.0f;

            for (int i = 0; i < 2; i++)
            {
                h = -height / 2.0f + i * height;
                sectorAngle = 0.0f;

                mVertices.emplace_back(0, h, 0); 
                mTextureCoordinates.push_back({0.5f, 0.5f});

                for (u32 j = 0; j < sectorCount; j++)
                {
                    sectorAngle = j * sectorStep;
                    float cosAngle = std::cos(sectorAngle);
                    float sinAngle = std::sin(sectorAngle);
                    mVertices.emplace_back(
                            cosAngle * radius, 
                            h, sinAngle * radius);
                    mTextureCoordinates.push_back(
                            {(float) j / sectorCount, 
                            (float)(1 - i)});
                }

            }
            for (int i = 1; i < sectorCount + 1; i++)
            {
                mIndices.push_back(i * (i != sectorCount) + 1);
                mIndices.push_back(i);
                mIndices.push_back(sectorCount + i + 1);

                mIndices.push_back(i * (i != sectorCount) + 1);
                mIndices.push_back(sectorCount + i + 1);
                mIndices.push_back(sectorCount + 2 + i * (i != sectorCount));

                mIndices.push_back((i + 1) * !(i == sectorCount) + (i == sectorCount));
                mIndices.push_back(i);
                mIndices.push_back(0);

                mIndices.push_back(sectorCount + 1);
                mIndices.push_back(i + sectorCount + 1);
                mIndices.push_back(sectorCount + 2 + i * (i != sectorCount));
            }
            computeFullVertices();
        }
        
}}}

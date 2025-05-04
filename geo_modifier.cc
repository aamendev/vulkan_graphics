#include "geo_modifier.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "core/utils.h"

namespace Lina { namespace Graphics {
    void GeoModifier::fbm(f32 h, f32 lac, f32 octaves)
    {
        if (!mInit)[[unlikely]]
            init_fbm(h, lac);
        Math::Point3D currentPoint;
        Math::Vector3D currentNormal;
        f32 currentOut;
        for (int i = 0; i < mVertices.size(); i+=3)
        {

            currentPoint = {mVertices[i], mVertices[i + 1], mVertices[i+2]};
            currentOut = perPointFbm(currentPoint, h, lac, octaves); 
            switch(mNormalsMode)
            {
                case ModifierNormals::Ignore:
                currentPoint.y += currentOut * mMulFactor;
                break;
                case ModifierNormals::Provided:
                {
                    currentNormal = mNormals[i/3];
                    currentNormal = currentNormal.normalise();
                    currentPoint += (currentNormal * (currentOut * mMulFactor));
                }
                break;
                case ModifierNormals::Compute:
                break;
            }

            mVertices[i] = currentPoint.x;
            mVertices[i+1] = currentPoint.y;
            mVertices[i+2] = currentPoint.z;
        }
    }

    void GeoModifier::hybridMultiFractal(f32 h, f32 lac, f32 octaves, f32 offset)
    {
        if (!mInit)[[unlikely]]
            init_fbm(h, lac);
        Math::Point3D currentPoint;
        Math::Vector3D currentNormal;
        f32 currentOut;
        for (int i = 0; i < mVertices.size(); i+=3)
        {

            currentPoint = {mVertices[i], mVertices[i + 1], mVertices[i+2]};
            currentOut = perPointHybridMultiFractal(currentPoint, h, lac, octaves, offset); 
            switch(mNormalsMode)
            {
                case ModifierNormals::Ignore:
                currentPoint.y += currentOut * mMulFactor;
                break;
                case ModifierNormals::Provided:
                {
                    currentNormal = mNormals[i/3];
                    //currentNormal = currentNormal.normalise();
                    currentPoint += (currentNormal * (currentOut * mMulFactor));
                }
                break;
                case ModifierNormals::Compute:
                break;
            }

            mVertices[i] = currentPoint.x;
            mVertices[i+1] = currentPoint.y;
            mVertices[i+2] = currentPoint.z;
        }
    }

    void GeoModifier::warp(f32 h, f32 lac, f32 octaves, f32 dist)
    {
        if (!mInit)[[unlikely]]
            init_fbm(h, lac);
        Math::Point3D currentPoint;
        Math::Vector3D currentNormal;
        f32 currentOut;
        u32 currentTriangle = 0;
        auto copyOfVerts = mVertices;
        for (int i = 0; i < mVertices.size(); i+=3)
        {
            currentPoint = {mVertices[i], mVertices[i + 1], mVertices[i+2]};
            currentOut = perPointWarp(currentPoint, h, lac, octaves, dist); 
            //currentPoint.y = currentOut * mMulFactor;
            switch(mNormalsMode)
            {
                case ModifierNormals::Ignore:
                currentPoint.y += currentOut * mMulFactor;
                break;
                case ModifierNormals::Provided:
                {
                    currentNormal = mNormals[i/3];
                    //currentNormal = currentNormal.normalise();
                    currentPoint += (currentNormal * (currentOut * mMulFactor));
                }
                break;
                case ModifierNormals::Compute:
                {
                    Math::Point3D p1 = {
                        copyOfVerts[currentTriangle], 
                        copyOfVerts[currentTriangle + 1], 
                        copyOfVerts[currentTriangle + 2]
                    };
                    Math::Point3D p2 =  {
                        copyOfVerts[currentTriangle + 3], 
                        copyOfVerts[currentTriangle + 3 + 1], 
                        copyOfVerts[currentTriangle + 3 + 2]};
                    Math::Point3D p3 = {
                        copyOfVerts[currentTriangle + (2*3)],
                        copyOfVerts[currentTriangle + (2*3) + 1],
                        copyOfVerts[currentTriangle + (2*3) + 2]};

                    currentNormal = (p2 - p1).cross(p3 - p1).normalise();
                    currentPoint += (currentNormal * (currentOut * mMulFactor));
                }
                break;
            }
            mVertices[i] = currentPoint.x;
            mVertices[i+1] = currentPoint.y;
            mVertices[i+2] = currentPoint.z;

            if (i%9 == 0 && i > 0)
            {
                for (int j = 0; j < 9; j++)
                {
                    copyOfVerts[i+j] = mVertices[i+j];
                }
            currentTriangle += 9;
            }
        }
    }

    void GeoModifier::combineVertices()
    {
        mFullVertices.resize(mNormals.size() * 6);
        int currIdx = 0;
        for (int i = 0; i < mNormals.size(); i++)
        {
            mFullVertices[currIdx++] = mVertices[i*3];
            mFullVertices[currIdx++] = mVertices[i*3 + 1];
            mFullVertices[currIdx++] = mVertices[i*3 + 2];
            mFullVertices[currIdx++] = mNormals[i].x;
            mFullVertices[currIdx++] = mNormals[i].y;
            mFullVertices[currIdx++] = mNormals[i].z;
        }
    }
}}

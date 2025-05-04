#include "color_modifier.h"
namespace Lina { namespace Graphics {
    void ColModifier::fbm(f32 h, f32 lac, f32 octaves)
    {
        if (!mInit)[[unlikely]]
            init_fbm(h, lac);
        Math::Point3D currentPoint;
        Math::Vector3D currentNormal;
        f32 currentOut;
        mAlphas.clear();
        mAlphas.reserve(mVertices.size() / 3);
        for (int i = 0; i < mVertices.size(); i+=3)
        {
            currentPoint = {mVertices[i], mVertices[i + 1], mVertices[i+2]};
            //currentOut = fabs(perPointFbm(currentPoint, h, lac, octaves)) * mMulFactor;
            currentOut = (0.5f * sinf(gnoise(currentPoint.x, currentPoint.y, currentPoint.z))
                    + 0.5f) * mMulFactor;
           mAlphas.push_back(currentOut);
        }
    }

    void ColModifier::combineVertices()
    {
        mFullVertices.resize(mAlphas.size() * 4);
        int currIdx = 0;
        for (int i = 0; i < mAlphas.size(); i++)
        {
            mFullVertices[currIdx++] = mVertices[i*3];
            mFullVertices[currIdx++] = mVertices[i*3 + 1];
            mFullVertices[currIdx++] = mVertices[i*3 + 2];
            mFullVertices[currIdx++] = mAlphas[i];
           // std::cerr << "ALPHA: " << mAlphas[i] << '\n';
        }
    }
}}

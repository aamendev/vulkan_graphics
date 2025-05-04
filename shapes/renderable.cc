#include "renderable.h"
#include <algorithm>

namespace Lina{ namespace Graphics{
    void Renderable::computeFullVertices()
    {
        switch(mMeshMode)
        {
            case MeshMode::Pos3Col3Tex2:
                {
                    mFullVertices.resize(mVertices.size() * 8);
                    int currIndex = 0;
                    for (int i = 0; i < mVertices.size(); i++)
                    {
                        mFullVertices[currIndex++] = mVertices[i].x;
                        mFullVertices[currIndex++] = mVertices[i].y;
                        mFullVertices[currIndex++] = mVertices[i].z;
                        mFullVertices[currIndex++] = mMaterial.col.x;
                        mFullVertices[currIndex++] = mMaterial.col.y; 
                        mFullVertices[currIndex++] = mMaterial.col.z; 
                        mFullVertices[currIndex++] = (mTextureCoordinates[i].u);
                        mFullVertices[currIndex++] = (mTextureCoordinates[i].v);
                    }
                }
                break;

            case MeshMode::Pos3:
                {
                    std::cerr << "POS3\n";
                    mFullVertices.resize(mVertices.size() * 3);
                    for (int i = 0; i < mVertices.size(); i++)
                    {
                        mFullVertices[3 * i] = mVertices[i].x;
                        mFullVertices[3 * i + 1] = mVertices[i].y;
                        mFullVertices[3 * i + 2] = mVertices[i].z;
                    }
                }
                break;

            case MeshMode::Pos3Col3:
                {
                    mFullVertices.resize(mVertices.size() * 6);
                    int currIndex = 0;
                    for (int i = 0; i < mVertices.size(); i++)
                    {
                        mFullVertices[currIndex++] = mVertices[i].x;
                        mFullVertices[currIndex++] = mVertices[i].y;
                        mFullVertices[currIndex++] = mVertices[i].z;
                        mFullVertices[currIndex++] = mMaterial.col.x;
                        mFullVertices[currIndex++] = mMaterial.col.y; 
                        mFullVertices[currIndex++] = mMaterial.col.z; 
                    }
                }
                break;
            case MeshMode::Pos3Norm3:
                {
                    mFullVertices.resize(mVertices.size() * 6);
                    int currIndex = 0;
                    for (int i = 0; i < mVertices.size(); i++)
                    {
                        mFullVertices[currIndex++] = mVertices[i].x;
                        mFullVertices[currIndex++] = mVertices[i].y;
                        mFullVertices[currIndex++] = mVertices[i].z;
                        mFullVertices[currIndex++] = mNormals[i].x;
                        mFullVertices[currIndex++] = mNormals[i].y; 
                        mFullVertices[currIndex++] = mNormals[i].z; 
                    }
                }
            default: 
                {

                }
        }
    }
    std::vector<float> Renderable::getTextureDebug() const
    {
        std::vector<float> tex(mTextureCoordinates.size() * 8, 0);

        int currIndex = 0;
        std::cerr << std::max_element(mTextureCoordinates.begin(), mTextureCoordinates.end(),
                [](const uv uv0, const uv uv1){return uv0.u < uv1.u;})->u<<',';

        std::cerr << std::max_element(mTextureCoordinates.begin(), mTextureCoordinates.end(),
                [](const uv uv0, const uv uv1){return uv0.v < uv1.v;})->v<<'\n';
        for (int i = 0; i < mTextureCoordinates.size(); i++)
        {
            tex[currIndex++] = (mTextureCoordinates[i].u) - 0.5;
            tex[currIndex++] = (mTextureCoordinates[i].v) - 0.5;
            tex[currIndex++] = 0; 
            tex[currIndex++] = 1.0f;
            tex[currIndex++] = 1.0f;
            tex[currIndex++] = 1.0f;
            tex[currIndex++] = (mTextureCoordinates[i].u);
            tex[currIndex++] = (mTextureCoordinates[i].v);
        }
        return tex;
    }
}}


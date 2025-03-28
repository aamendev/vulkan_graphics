#include "tiny_based_obj_loader.h"
#include <vector>

namespace Lina { namespace Core {
    b8 TinyBasedObjLoader::load(std::string&& path)
    {
        mVertices.clear();
        mIndices.clear();
        mFullVertices.clear();
        mTexture.clear();
        mColors.clear();
        mNormals.clear();
        std::string warn, err;
        tinyobj::ObjReader r;
        r.ParseFromFile(path, {});
        tinyobj::attrib_t attrib = r.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = r.GetShapes();
        std::vector<tinyobj::material_t> materials = r.GetMaterials();
        for (const auto& s : shapes)
        {
            for (const auto& idx : s.mesh.indices)
            {
                mIndices.push_back(mIndices.size());
                for (int i = 0; i < 3; i++)
                {
                    mVertices.push_back(attrib.vertices[3 * idx.vertex_index + i]);
                    mFullVertices.push_back(attrib.vertices[3 * idx.vertex_index + i]);
                }
                for (int i = 0; i < 2; i++)
                {
                    if (attrib.texcoords.empty())
                    {
                        mTexture.push_back(0);
                        mFullVertices.push_back(0);
                    }
                    else 
                    {
                    mTexture.push_back(attrib.texcoords[2 * idx.texcoord_index + i]);
                    mFullVertices.push_back(attrib.texcoords[2 * idx.texcoord_index + i]);
                    }
                }
                for (int i = 0; i < 3; i++)
                {
                    if (attrib.normals.empty())
                    {
                        mNormals.push_back(0);
                        mFullVertices.push_back(0);
                    }
                    else
                    {
                    mNormals.push_back(attrib.normals[3 * idx.normal_index + i]);
                    mFullVertices.push_back(attrib.normals[3 * idx.normal_index + i]);
                    }
                }
                for (int i = 0; i < 3; i++)
                {
                    if (s.mesh.material_ids.empty() || materials.empty())
                    {
                        mColors.push_back(1);
                        mFullVertices.push_back(1);
                    }
                    else
                    {
                    //mColors.push_back(materials[s.mesh.material_ids[0]].diffuse[i]);
                    //
                    mFullVertices.push_back(materials[s.mesh.material_ids[0]].diffuse[i]);
                    }
                }
            }
        }
        return true;
    }
}}

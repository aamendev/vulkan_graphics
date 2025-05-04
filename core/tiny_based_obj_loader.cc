#include "tiny_based_obj_loader.h"
#include <vector>
#include <set>

namespace Lina { namespace Core {
    b8 TinyBasedObjLoader::load(std::string&& path)
    {
        mPath = path;
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
        std::set<u32> tempIndices = {};
        mVertices.resize(attrib.GetVertices().size());
        for (const auto& s : shapes)
        {
            for (const auto& idx : s.mesh.indices)
            {
                auto [_, inserted] = tempIndices.insert(idx.vertex_index);
                mIndices.push_back(idx.vertex_index);
                auto currIdx = 3 * idx.vertex_index;
                if (inserted)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        mVertices[currIdx+i] = attrib.vertices[currIdx + i];
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
                            mFullVertices.push_back(materials[s.mesh.material_ids[0]].diffuse[i]);
                        }
                    }
                }
            }
        }
        return true;
    }

    void TinyBasedObjLoader::debug()
    {
        std::cerr << "VERTS: \n";
        for (int i = 0; i < mVertices.size(); i++)
        {
            std::cerr << mVertices[i] << ", ";
            if (i%3 == 2) std::cerr << '\n';
        }
        std::cerr << "END VERTS\n";

        std::cerr << "Inds: \n";
        for (int i = 0; i < mIndices.size(); i++)
        {
            std::cerr << mIndices[i] << ", ";
            if (i%3 == 2) std::cerr << '\n';
        }
        std::cerr << "END Inds\n";
    }

    Helpers::AI::Graph TinyBasedObjLoader::getGraph()
    {
        using namespace Helpers::AI;
        std::string warn, err;
        tinyobj::ObjReader r;
        r.ParseFromFile(mNavPath, {});
        tinyobj::attrib_t attrib = r.GetAttrib();
        std::vector<tinyobj::shape_t> shapes = r.GetShapes();
        std::array<u32, 3> trangleVerts;
        std::vector<std::array<u32, 3>> faces;
        std::array<Math::Point3D, 3> tempVerts;
        Graph g;
        for (const auto& s : shapes)
        {
            for (int face = 0; face < s.mesh.indices.size(); face += 3)
            {
                AStarNode* currNode = new AStarNode;
                for (int i = 0; i < 3; i++)
                {
                    trangleVerts[i] = 
                        s.mesh.indices[face + i].vertex_index;
                }
                currNode->x = 
                    (attrib.vertices[3 * trangleVerts[0] + 0] +
                    attrib.vertices[3 * trangleVerts[1] + 0]) / 2;
                currNode->y = 
                    (attrib.vertices[3 * trangleVerts[0] + 1] +
                    attrib.vertices[3 * trangleVerts[1] + 1]) / 2;
                currNode->z = 
                    (attrib.vertices[3 * trangleVerts[0] + 2] +
                    attrib.vertices[3 * trangleVerts[1] + 2]) / 2;
                currNode->id = faces.size();
                g.addNode({currNode, {}});
                faces.push_back(trangleVerts);
            }
        }
        for (int i = 0; i < faces.size(); i++)
        {
            for (int j = 0;  j < faces.size(); j++)
            {
                if (shares_edge(faces[i], faces[j]))
                {
                    g[i].second.push_back(
                            {distance(g[i].first, g[j].first),
                            g[j].first});
                }
            }
        }
        return g;
    }
    b8 TinyBasedObjLoader::shares_edge(std::array<u32, 3>& a1, std::array<u32, 3>& a2)
    {
        int equal_count = 0;
        for (auto first : a1)
        {
            for (auto second: a2)
            {
                equal_count += (first == second);
            }
        }
        return equal_count == 2;
    }
    f32 TinyBasedObjLoader::distance(Helpers::AI::AStarNode* a1, Helpers::AI::AStarNode* a2)
    {
        return 
            ((Math::Vector3D{a1->x, a1->y, a1->z}) - 
            (Math::Vector3D{a2->x, a2->y, a2->z})).squaredMagnitude();
    }
}}

#include "collision_funcs.h"
#include <vector>
namespace Lina { namespace Helpers { namespace Collisions{
    using namespace ECS::Components;

    Math::Vector3D support(Collider* a, Collider* b, const Math::Vector3D& d)
    {
       return a->furthestPoint(d) - b->furthestPoint(-d);
    }

    b8 gjk(Collider* a, Collider* b)
    {
        Simplex mSimplex;
        Math::Vector3D sup = support(a, b, {0, 1, 0});
        mSimplex.push_front(sup); 
        Math::Vector3D newDir = -sup;

        while(true)
        {
            sup = support(a, b, newDir);
            if (sup.dot(newDir) <= 0) {mSimplex.clear(); return false;}
            mSimplex.push_front(sup);
            if (checkNextSimplex(mSimplex, newDir)) 
            {
                return true;
            };
        }
    }

    Info gjkAndInfo(Collider* a, Collider* b)
    {
        Simplex mSimplex;
        Math::Vector3D sup = support(a, b, {0, 1, 0});
        mSimplex.push_front(sup); 
        Math::Vector3D newDir = -sup;

        while(true)
        {
            sup = support(a, b, newDir);
            if (sup.dot(newDir) <= 0) {mSimplex.clear(); return {.collided = false};}
            mSimplex.push_front(sup);
            if (checkNextSimplex(mSimplex, newDir)) 
            {
                //return {.depth = 0.0, .normal = {0, 0, 0}, .collided = true};
                return epa(mSimplex, a, b);
            };
        }
    }


    b8 checkNextSimplex(Simplex& mSimplex, Math::Vector3D& d)
    {
        switch(mSimplex.size)
        {
            case 2: return checkLine(mSimplex, d);
            case 3: return checkTri(mSimplex, d);
            case 4: return checkTetra(mSimplex, d); break;
            default:
                    {
            //            std::cerr << "Error, Simplex with bad size: " << mSimplex.size;
                        std::cerr << "\n";
                        return false;
                    }
                    break;
        }
        return false;
    }

    b8 checkLine(Simplex& mSimplex, Math::Vector3D& d)
    {
        Math::Vector3D ab = mSimplex.at(1) - mSimplex.at(0); 
        Math::Vector3D ao = -mSimplex.at(0);
        if (ab.sameDirection(ao))
        {
            d = (ab.cross(ao)).cross(ab);
        }
        else
        {
            d = ao;
            mSimplex.set({mSimplex.at(0)});
        }
        return false;
    }

    b8 checkTri(Simplex& mSimplex, Math::Vector3D& d)
    {
        Math::Vector3D ab = mSimplex.at(1) - mSimplex.at(0);
        Math::Vector3D ac = mSimplex.at(2) - mSimplex.at(0);
        Math::Vector3D ao = -mSimplex.at(0);
        
        Math::Vector3D abc = ab.cross(ac);

        if (ao.sameDirection(abc.cross(ac)))
        {
            if (ao.sameDirection(ac))
            {
                mSimplex.set({mSimplex.at(0), mSimplex.at(2)});
                d = ac.cross(ao).cross(ac);
            }
            else 
            {
                mSimplex.set({mSimplex.at(0), mSimplex.at(1)});
                return checkLine(mSimplex, d);
            }
        }
        else 
        {
            if (ao.sameDirection(ab.cross(abc))) 
            {

                mSimplex.set({mSimplex.at(0), mSimplex.at(1)});
                return checkLine(mSimplex, d);
            }
            else if (ao.sameDirection(abc))
            {
                d = abc;
            }
            else
            {
                mSimplex.set({mSimplex.at(0), mSimplex.at(2), mSimplex.at(1)});
                d = -abc;
            }
        }

        return false;
    }

    b8 checkTetra(Simplex& mSimplex, Math::Vector3D& d)
    {
        Math::Vector3D ab = mSimplex.at(1) - mSimplex.at(0);
        Math::Vector3D ac = mSimplex.at(2) - mSimplex.at(0);
        Math::Vector3D ad = mSimplex.at(3) - mSimplex.at(0);
        Math::Vector3D ao =  -mSimplex.at(0);

        Math::Vector3D abc = ab.cross(ac);
        Math::Vector3D acd = ac.cross(ad);
        Math::Vector3D adb = ad.cross(ab);

        if (ao.sameDirection(abc))
        {
            mSimplex.set({mSimplex.at(0), mSimplex.at(1), mSimplex.at(2)});
            return checkTri(mSimplex, d);
        }

        if (ao.sameDirection(acd))
        {
            mSimplex.set({mSimplex.at(0), mSimplex.at(2), mSimplex.at(3)});
            return checkTri(mSimplex, d);
        }

        if (ao.sameDirection(adb))
        {
            mSimplex.set({mSimplex.at(0), mSimplex.at(3), mSimplex.at(1)});
            return checkTri(mSimplex, d);
        }
            return true;
    }


    Info epa(Simplex& s, Collider* a, Collider* b)
    {
        std::vector<Math::Vector3D> verts(s.begin(), s.end());
        std::vector<u32> ind
        {
            0, 1, 2,
            0, 3, 1,
            0, 2, 3, 
            1, 3, 2
        };

        auto [normals, minFace] = normalAndMin(verts, ind);
        Math::Vector3D minNormal;
        f32 minDist = 0.0f;
        f32 oldDist = 0.0f;
        u32 iterations = 10;
        while (iterations > 0)
        {
            minNormal = {normals[minFace].x, normals[minFace].y, normals[minFace].z};
            minDist = normals[minFace].w;

            Math::Vector3D sup = support(a, b, minNormal);
            f32 dist = minNormal.dot(sup);

            if (fabs(dist - minDist) > COLLISION_EPSILON)
            {
                if (fabs(dist - oldDist) < COLLISION_EPSILON)
                {
                    iterations--;
                }
                else {
                    oldDist = dist;
                iterations = 10;
                }
                std::vector<std::pair<u32, u32>> edges;
                for (u32 i = 0; i < normals.size(); i++)
                {
                    Math::Vector3D n3 = {normals[i].x, normals[i].y, normals[i].z};
                    u32 f = i * 3;
                    if (n3.sameDirection(sup - verts[ind[f]]))
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            addUniqueRemoveDup(edges, ind, f + j, f + ((j + 1) % 3));
                        }
                        for (int j = 2; j > -1; j--)
                        {
                            ind[f + j] = ind.back(); ind.pop_back();
                        }
                        normals[i] = normals.back(); normals.pop_back();
                        i--;
                    }
                }
                std::vector<u32> newInd;
                for (auto [v1, v2] : edges)
                {
                    newInd.push_back(v1);
                    newInd.push_back(v2);
                    newInd.push_back(verts.size());
                }
                verts.push_back(sup);
                auto [newNormals, newMinFace] = normalAndMin(verts, newInd);

                f32 oldMin = FLT_MAX;
                for (u32 i = 0; i < normals.size(); i++)
                {
                    if (normals[i].w < oldMin)
                    {
                        oldMin = normals[i].w;
                        minFace = i;
                    }
                }
                if (newNormals[minFace].w < oldMin)
                {
                    minFace = newMinFace + normals.size();
                }
                ind.insert(ind.end(), newInd.begin(), newInd.end());
                normals.insert(normals.end(), newNormals.begin(), newNormals.end());
            }
            else
            {
                break;
            }
        }
       // minDist = minDist * (iterations > 0) + oldDist * !(iterations > 0);
        Info inf {
            .depth = minDist/* + COLLISION_EPSILON*/,
            .normal = minNormal,
            .collided = true,
        };
        return inf;
    }
    std::pair<std::vector<Math::Vector4D>, u32> normalAndMin(
            std::vector<Math::Vector3D>& verts,
            std::vector<u32>& indices)
    {
        std::vector<Math::Vector4D> normals;
        u32 minFace = 0;
        f32 minDist = FLT_MAX;

        for (u32 i = 0; i < indices.size(); i+=3)
        {
            Math::Vector3D p0 =  verts[indices[i]];
            Math::Vector3D p1 =  verts[indices[i + 1]];
            Math::Vector3D p2 =  verts[indices[i + 2]];
            Math::Vector3D norm = ((p1 - p0).cross(p2 - p0)).normalise();
            f32 dist = norm.dot(p0);
            norm *= (-(dist < 0) + !(dist < 0));
            dist *= (-(dist < 0) + !(dist < 0));
            normals.emplace_back(norm.x, norm.y, norm.z, dist);
            if (dist < minDist)
            {
                minFace = i/3 * (dist < minDist) + minFace * !(dist < minDist);
                minDist = dist * (dist < minDist) + dist * !(dist < minDist);
            }
        }
        return {normals, minFace};
    }
    void addUniqueRemoveDup(std::vector<std::pair<u32, u32>>& edges, 
            std::vector<u32>& ind,
            u32 a, u32 b)
    {
        auto e = std::find(edges.begin(), edges.end(), std::make_pair(ind[b], ind[a]));
        if (e != edges.end()) edges.erase(e);
        else edges.emplace_back(ind[a], ind[b]);
    }
}}}

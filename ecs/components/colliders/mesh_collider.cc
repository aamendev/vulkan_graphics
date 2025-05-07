#include "mesh_collider.h"
#include "box_collider.h"
#include "collider.h"
#include <algorithm>
#include <cfloat>
#include <cmath>
namespace Lina{ namespace ECS { namespace Components { namespace Colliders{
    Math::Point3D Mesh::furthestPoint(const Math::Vector3D& d)
    {
        Math::Transform4D transform = 
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D() *
             Math::Util::scaleMatrix(mScale);

        Math::Vector4D temp = (Math::Vector4D){d.x, d.y, d.z, 1} * transform.transpose();
        Math::Vector3D newDir = {temp.x, temp.y, temp.z};

        Math::Point3D furtherPoint = {0, 0, 0};
        Math::Point3D current = {0, 0, 0};
        f32 maxDistance = -FLT_MAX;

        for (int i = 0; i + 2 < mVertices.size(); i+=3)
        {
            current = {mVertices[i], mVertices[i + 1], mVertices[i + 2]};
           f32 dotProd = current.dot(newDir); 

           if (dotProd > maxDistance)
           {
                furtherPoint = current;
                maxDistance = dotProd;
           }
        }
        auto ret =  (furtherPoint * transform + mCenter).toPoint();
        return ret; 
    }

    Math::Vector3D Mesh::support(Collider* a, Collider* b, const Math::Vector3D& d)
    {
       return a->furthestPoint(d) - b->furthestPoint(-d);
    }

    b8 Mesh::gjk(Collider* a, Collider* b)
    {
        Math::Vector3D sup = support(a, b, {0, 1, 0});
        mSimplex.push_front(sup); 
        Math::Vector3D newDir = -sup;

        while(true)
        {
            sup = support(a, b, newDir);
            if (sup.dot(newDir) <= 0) {mSimplex.clear(); return false;}
            mSimplex.push_front(sup);
            if (checkNextSimplex(newDir)) {mSimplex.clear(); return true;};
        }
    }

    b8 Mesh::checkNextSimplex(Math::Vector3D& d)
    {
        switch(mSimplex.size)
        {
            case 2: return checkLine(d);
            case 3: return checkTri(d);
            case 4: return checkTetra(d); break;
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

    b8 Mesh::checkLine(Math::Vector3D& d)
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

    b8 Mesh::checkTri(Math::Vector3D& d)
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
                return checkLine(d);
            }
        }
        else 
        {
            if (ao.sameDirection(ab.cross(abc))) 
            {

                mSimplex.set({mSimplex.at(0), mSimplex.at(1)});
                return checkLine(d);
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

    b8 Mesh::checkTetra(Math::Vector3D& d)
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
            return checkTri(d);
        }

        if (ao.sameDirection(acd))
        {
            mSimplex.set({mSimplex.at(0), mSimplex.at(2), mSimplex.at(3)});
            return checkTri(d);
        }

        if (ao.sameDirection(adb))
        {
            mSimplex.set({mSimplex.at(0), mSimplex.at(3), mSimplex.at(1)});
            return checkTri(d);
        }
            return true;
    }

    void Mesh::onCollisionEnter(Collider *c){
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void Mesh::onCollisionPersist(Collider *c)
    {
        mCollisionPersistCallback(this, c);
        if (mCallDefaults)
            defaultOnCollisionPersist(c);
    }

    void Mesh::onCollisionExit(Collider *c){
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionExit(c);
        }
    }
    void Mesh::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults)
            defaultOnResolve(c);
    }

    std::pair<Math::Point3D, Math::Point3D> Mesh::computeBoundingBox(u32 idxBegin, u32 idxEnd)
    {
        Math::Transform4D transform = 
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D()*
             Math::Util::scaleMatrix(mScale);
        f32 minX = FLT_MAX; 
        f32 minY = FLT_MAX; 
        f32 minZ = FLT_MAX; 
        f32 maxX = -FLT_MAX; 
        f32 maxY = -FLT_MAX; 
        f32 maxZ = -FLT_MAX; 
        Math::Vector3D point;
        for (int i = idxBegin; i < idxEnd - 2; i+=3)
        {
            point = 
                (Math::Point3D(mVertices[i], mVertices[i+1], mVertices[i+2])) * transform + mCenter;

             minX = minX * (minX < point.x) + point.x * !(minX < point.x);
            minY = minY * (minY < point.y) + point.y * !(minY < point.y);
            minZ = minZ * (minZ < point.z) + point.z * !(minZ < point.z);

            maxX = maxX * (maxX > point.x) + point.x * !(maxX > point.x);
            maxY = maxY * (maxY > point.y) + point.y * !(maxY > point.y);
            maxZ = maxZ * (maxZ > point.z) + point.z * !(maxZ > point.z);
        }
        auto boundingBoxExtentsFirst = 
            (((Math::Point3D){minX, minY, minZ})).toPoint();

        auto BoundingBoxExtentsSecond = 
            (((Math::Point3D){maxX, maxY, maxZ})).toPoint();

        return std::make_pair(boundingBoxExtentsFirst, BoundingBoxExtentsSecond);
    }

    void Mesh::computeBoundingBox()
    {
        mBoundingBoxExtents = computeBoundingBox(0, mVertices.size());
    }

    Collider::BVH* Mesh::computeBVH(u32 begin, u32 end)
    {
        auto boundingBox = computeBoundingBox(begin, end); 
        Box* boundCollider = new Box(mTag, boundingBox);
        boundCollider->computeBoundingBox();
        BVH* node = new BVH;
        node->root = boundCollider;
        return node;
    }

    Collider::BVH* Mesh::computeBVH(u32 begin)
    {
        Mesh* leaf = new Mesh(mTag);
        std::vector<f32> verts;
        std::vector<u32> inds;
        verts.reserve(9);
        inds.reserve(3);

        for (int j = 0; j < 3; j++)
        {
            inds[j] = j;
            auto idx = mIndices[begin + j];
            //inds[j] = 2 * (idx == *max) + (idx != *max && idx != *min);
        for (int i = 0; i < 3; i++)
        {
            verts.push_back(mVertices[3 * idx + i]);
        }
        }

        leaf->setVertices(verts);
        leaf->setIndices(inds);
        leaf->setScale(mScale);
        leaf->setRotation(mRotation);
        leaf->setPosition(mCenter);
        leaf->computeBoundingBox();
        BVH* node = new BVH;
        node->root = leaf;
        return node;
    }

    void Mesh::computeBVH()
    {
        std::vector<BVH*> mbvhs;
        mbvhs.reserve(mIndices.size() / 3);
        for (int i = 0; i < mIndices.size(); i+=3)
        {
           mbvhs.push_back(computeBVH(i)); 
        }

        int n = mbvhs.size();
        int power = 1;
        b8 reverse = false; // to balance tree
        int final = 0;
        while (power < n)
        {
            if (!reverse)
            {
                for (int i = 0; i < n; i += power<<1)
                {

                 if (i + power < n)[[likely]]
                 {
                    combineBVHs(i, i + power, mbvhs);
                    final = i+1;
                 }
                 else 
                 {
                    combineBVHs(0, i, mbvhs);
                    final = i - (power<<1) + 1;
                 }
                }
                n = final;
            }
            else if (reverse)
            {
                for (int i = ((n-1) / power) * power; i >= power; i -= power<<1)
                {
                    combineBVHs(mbvhs[i-power], mbvhs[i]);
                }
            }
            power <<= 1;
        }
        mBvh = *mbvhs[0];
        delete mbvhs[0];
    }
    void Mesh::computeBBCompound()
    {
        computeBVH();
        mBoundingBoxExtents = mBvh.root->getBoundingBox();
    }
void Mesh::combineBVHs(u32 idx0, u32 idx1, std::vector<Collider::BVH*>& mbvhs)
    {
        if (mbvhs[idx1] == nullptr) return;
        if (mbvhs[idx0] == nullptr || mbvhs[idx0]->root == nullptr)
        {
            std::cerr << "Fatal Error, b1 is nullptr\n";
            exit(1);
        }
        BVH* newLeft = new BVH;
        mbvhs[idx0]->root->computeBoundingBox();
        mbvhs[idx1]->root->computeBoundingBox();
        auto leftBoundingBox = mbvhs[idx0]->root->getBoundingBox();
        auto rightBoundingBox = mbvhs[idx1]->root->getBoundingBox();

        std::pair<Math::Point3D, Math::Point3D> combinedBound = 
            std::make_pair(Math::Point3D(
                        std::fmin(leftBoundingBox.first.x, rightBoundingBox.first.x),
                        std::fmin(leftBoundingBox.first.y, rightBoundingBox.first.y),
                        std::fmin(leftBoundingBox.first.z, rightBoundingBox.first.z)
                        ), Math::Point3D(
                        std::fmax(leftBoundingBox.second.x, rightBoundingBox.second.x),
                        std::fmax(leftBoundingBox.second.y, rightBoundingBox.second.y),
                        std::fmax(leftBoundingBox.second.z, rightBoundingBox.second.z)
                            ));
        
        Box* boundCollider = new Box(mTag, combinedBound);
        boundCollider->computeBoundingBox();
        newLeft->root = mbvhs[idx0]->root;
        newLeft->left = mbvhs[idx0]->left;
        newLeft->right = mbvhs[idx0]->right;
        mbvhs[idx0]->left = newLeft;
        mbvhs[idx0]->right = mbvhs[idx1];
        mbvhs[idx0]->root = boundCollider;
    }
    void Mesh::combineBVHs(BVH* b1, BVH* b2)
    {
        if (b2 == nullptr) return;
        if (b1 == nullptr)
        {
            std::cerr << "Fatal Error, b1 is nullptr\n";
            exit(1);
        }
        BVH* newLeft = new BVH;
        b1->root->computeBoundingBox();
        b2->root->computeBoundingBox();
        auto leftBoundingBox = b1->root->getBoundingBox();     
        auto rightBoundingBox = b2->root->getBoundingBox();     

        std::pair<Math::Point3D, Math::Point3D> combinedBound = 
            std::make_pair(Math::Point3D(
                        std::fmin(leftBoundingBox.first.x, rightBoundingBox.first.x),
                        std::fmin(leftBoundingBox.first.y, rightBoundingBox.first.y),
                        std::fmin(leftBoundingBox.first.z, rightBoundingBox.first.z)
                        ), Math::Point3D(
                        std::fmax(leftBoundingBox.second.x, rightBoundingBox.second.x),
                        std::fmax(leftBoundingBox.second.y, rightBoundingBox.second.y),
                        std::fmax(leftBoundingBox.second.z, rightBoundingBox.second.z)
                            ));



        Box* boundCollider = new Box(mTag, combinedBound);
        boundCollider->computeBoundingBox();
        newLeft->root = b1->root;
        newLeft->left = b1->left;
        newLeft->right = b1->right;
        b1->left = newLeft;
        b1->right = b2;
        b1->root = boundCollider;
    }
}}}}

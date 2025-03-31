#include "mesh_collider.h"
#include "box_collider.h"
#include <cfloat>
namespace Lina{ namespace ECS { namespace Components { namespace Colliders{
    Math::Point3D Mesh::furthestPoint(const Math::Vector3D& d)
    {
        Math::Transform4D transform = 
             Math::Util::scaleMatrix(mScale)*
                    Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D();

        Math::Vector4D temp = (Math::Vector4D){d.x, d.y, d.z, 1} * transform.transpose();
        Math::Vector3D newDir = {temp.x, temp.y, temp.z};

        Math::Point3D furtherPoint = {0, 0, 0};
        Math::Point3D current = {0, 0, 0};
        f32 maxDistance = -FLT_MAX;

        for (int i = 0; i < mVertices.size(); i+=3)
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
             Math::Util::scaleMatrix(mScale)*
                    Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D();
        f64 minX, minY, minZ = FLT_MAX;
        f64 maxX, maxY, maxZ = -FLT_MAX;
        for (int i = idxBegin; i < idxEnd - 2; i+=3)
        {
            minX = minX * (minX < mVertices[i]) + mVertices[i] * !(minX < mVertices[i]);
            minY = minY * (minY < mVertices[i + 1]) + mVertices[i + 1] * !(minY < mVertices[i + 1]);
            minZ = minZ * (minZ < mVertices[i + 2]) + mVertices[i + 2] * !(minZ < mVertices[i + 2]);

            maxX = maxX * (maxX > mVertices[i]) + mVertices[i] * !(maxX > mVertices[i]);
            maxY = maxY * (maxY > mVertices[i + 1]) + mVertices[i + 1] * !(maxY > mVertices[i + 1]);
            maxZ = maxZ * (maxZ > mVertices[i + 2]) + mVertices[i + 2] * !(maxZ > mVertices[i + 2]);
        }
        auto boundingBoxExtentsFirst = 
            (((Math::Point3D){(f32)minX, (f32)minY, (f32)minZ} *
            transform) + mCenter).toPoint();

        auto BoundingBoxExtentsSecond = 
            (((Math::Point3D){(f32)maxX, (f32)maxY, (f32)maxZ} *
            transform) + mCenter).toPoint();

        return std::make_pair(boundingBoxExtentsFirst, BoundingBoxExtentsSecond);
    }

    void Mesh::computeBoundingBox()
    {
        //std::cerr << "Begin";
        mBoundingBoxExtents = computeBoundingBox(0, mVertices.size());
       // std::cerr << "End";
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
        Math::Transform4D m = 
            Math::Util::scaleMatrix(mScale)*
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D(); 

        leaf->setVertices({mVertices[begin], mVertices[begin+1], mVertices[begin+2]});
        leaf->setScale(getScale());
        leaf->setPosition((
                    (Math::Point3D){mVertices[begin], mVertices[begin+1], mVertices[begin+2]} 
                    * 1.0f/3 *  m 
                    + mCenter).toPoint());
        leaf->setRotation(getRotation());
        leaf->computeBoundingBox();
        BVH* node = new BVH;
        node->root = leaf;
        return node;
    }

    void Mesh::computeBVH()
    {
        std::vector<BVH*> mbvhs;
        mbvhs.reserve(mVertices.size() / 3 + 1);
        for (int i = 0; i < mVertices.size(); i+=3)
        {
           mbvhs.push_back(computeBVH(i)); 
        }

        int n = mbvhs.size();
        int power = 1;
        b8 reverse = false; // to balance tree
        while (n > power)
        {
            if (!reverse)
            {
                for (int i = 0; i < n - power; i += power<<1)
                {
                    combineBVHs(mbvhs[i], mbvhs[i + power]);
                }
            }
            else if (reverse)
            {
                for (int i = ((n-1) / power) * power; i >= power; i -= power<<1)
                {
                    combineBVHs(mbvhs[i-power], mbvhs[i]);
                }
            }
            reverse = !reverse;
            power <<= 1;
        }
        mBvh = *mbvhs[0];
        delete mbvhs[0];
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
        newLeft->right =b1->right;
        b1->left = newLeft;
        b1->right = b2;
        b1->root = boundCollider;
    }
}}}}

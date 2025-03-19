#include "mesh_collider.h"
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
}}}}

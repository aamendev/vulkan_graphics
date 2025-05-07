#include "cylinder_collider.h"
#include "collider.h"
#include "plane_collider.h"
#include "primitive_collisions.h"
#include <cmath>
namespace Lina { namespace ECS { namespace Components { namespace Colliders{
    b8 Cylinder::checkCollision(Collider* c)
    {
        ColliderGeometry g = c->getColliderGeometry();
        switch (g)
        {
            case ColliderGeometry::Cylinder:
                return Helpers::Collisions::cylinderCylinderCollision(this, c);
                break;
            case ColliderGeometry::Plane:
                {
                    return Helpers::Collisions::planeCylinderCollision(c, this);
                }
                break;
            case ColliderGeometry::Mesh:
                return c->checkCollision(this);
                break;
            case ColliderGeometry::Box:
                return Helpers::Collisions::boxCylinderCollision(c, this);
                break;
            default: 
                {
                    return false;
                }
                break;
        }
    }

    void Cylinder::onCollisionEnter(Collider *c){
        //std::cerr << "Enter\n";
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void Cylinder::onCollisionPersist(Collider *c)
    {

       // std::cerr << "Persist\n";
        mCollisionPersistCallback(this, c);
        if (mCallDefaults)
            defaultOnCollisionPersist(c);
    }

    void Cylinder::onCollisionExit(Collider *c){
       // std::cerr << "Exi\n";
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionExit(c);
        }
    }
    void Cylinder::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults)
            defaultOnResolve(c);
    }

    void Cylinder::defaultOnCollisionExit(Collider* c)
    {

    }

    void Cylinder::defaultOnCollisionPersist(Collider* c)
    {
        defaultOnCollisionEnter(c);
    }

    void Cylinder::defaultOnCollisionEnter(Collider* c)
    {
        switch(c->getColliderGeometry())
        {
            case ColliderGeometry::Cylinder:
                {
                    Cylinder* other = dynamic_cast<Cylinder*>(c);
                    Math::Vector3D otherCenter = other->getCenter();

                    Math::Vector3D proj1 = {mCenter.x, 0, mCenter.z};
                    Math::Vector3D proj2 = {otherCenter.x, 0, otherCenter.z};

                    f32 mag = (proj1 - proj2).magnitude();
                    f32 len = mRadius + other->getRadius() - mag;

                    if (mag == 0) {proj1 += {0.1, 0, 0};}
                    mCenter += ((proj1 - proj2).normalise() * (len + 0.01f));
                    defaultOnResolve(c);
                }
                break;
            case ColliderGeometry::Plane:
                {
                    Plane* other = dynamic_cast<Plane*>(c);
                    Math::Vector3D otherCenter = other->getCenter();
                    mCenter += {0, (otherCenter.y - mCenter.y) - (mHeight / 2), 0};
                    defaultOnResolve(c);
                }
                break;
            default: {}
        }
    }

    void Cylinder::defaultOnResolve(Collider* c)
    {
        /*    switch(c->getColliderGeometry())
              {
              case ColliderGeometry::Cylinder:
              {
              std::cerr << "Current Center: " << mCenter << ", Other's Center" 
              << ((Cylinder*)c)->getCenter() << "\n";
              }
              break;
              case ColliderGeometry::Plane:
              {
              std::cerr << "Current Center: " <<mCenter << "Plane's Center"
              << ((Plane*)c)->getCenter() << "\n";
              }
              break;
              default: {std::cerr << "Just Resolved!\n";}*/
        //}
    }


    Math::Point3D Cylinder::furthestPoint(const Math::Vector3D &d)
    {
        mScale = {mRadius * 2, mHeight, mRadius * 2};
        Math::Transform4D m = 
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D() * 
        Math::Util::scaleMatrix(mScale);

        Math::Vector4D temp = 
            ((Math::Vector4D){d.x, d.y, d.z, 1}) * m.transpose();

        Math::Vector3D newDir = {temp.x, temp.y, temp.z};
        newDir = newDir.normalise();

        f32 delta = std::sqrt(newDir.x * newDir.x + newDir.z * newDir.z);
        f32 sgn = !(newDir.y < 0) - (newDir.y < 0);
        auto ret = (Math::Point3D){0 , 0, 0};
        if (delta > 0)
        {
            ret = (
                    (Math::Point3D){(1.0f / delta) 
                    * newDir.x * 1.0f/2, sgn * 1.0f/2, (1.0f / delta) * newDir.z * 1.0f/2} 
                    * m
                    + mCenter).toPoint();
          //  std::cerr << "CylFurt " << ret << '\n';
            return ret;
        }
        ret = ((Math::Point3D){0, sgn * 1.0f / 2, 0} 
                * m 
                + mCenter).toPoint();
        return ret;
    }

    void Cylinder::computeBoundingBox()
    {
        mScale = {mRadius*2, mHeight, mRadius*2};
        Math::Transform4D m = 
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D() * 
        Math::Util::scaleMatrix(mScale);

        mBoundingBoxExtents.first = 
            ((Math::Point3D(-0.5f, -0.5f, -0.5f) * m) + mCenter).toPoint();

        mBoundingBoxExtents.second = 
            ((Math::Point3D(0.5f, 0.5f, 0.5f) * m) + mCenter).toPoint();
    }

    void Cylinder::computeBVH()
    {
        //mBvh = (BVH){Box("temp", mBoundingBoxExtents), nullptr, nullptr};
        computeBoundingBox();
        mBvh = (BVH){this, nullptr, nullptr};
    }
}}}}

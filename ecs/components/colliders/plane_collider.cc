#include "collider.h"
#include "primitive_collisions.h"
#include "plane_collider.h"
namespace Lina { namespace ECS { namespace Components { namespace Colliders{

    b8 Plane::checkCollision(Collider* c) 
    {
        ColliderGeometry g = c->getColliderGeometry();
        switch (g)
        {
            case ColliderGeometry::Cylinder:
                return Helpers::Collisions::planeCylinderCollision(this, c);
                break;
            case ColliderGeometry::Plane:
                {
                    return Helpers::Collisions::planePlaneCollision(this, c);
                }
                break;
            case ColliderGeometry::Mesh:
                return c->checkCollision(this);
                break;
            case ColliderGeometry::Box:
                return Helpers::Collisions::boxPlaneCollision(c, this);
                break;
            default: 
                {
                    return false;
                }
                break;
        }
    }
    void Plane::onCollisionEnter(Collider *c){
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
            // defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void Plane::onCollisionPersist(Collider *c)
    {
        mCollisionPersistCallback(this, c);
    }

    void Plane::onCollisionExit(Collider *c){
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
            // defaultOnCollisionExit(c);
        }
    }
    void Plane::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults){}
        // defaultOnResolve(c);
    }
    Math::Point3D Plane::furthestPoint(const Math::Vector3D& d)
    {
        mScale = {mLength, 1, mWidth};
        Math::Transform4D m = 
            Math::Util::scaleMatrix(mScale)*
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D(); 

        Math::Vector4D temp = 
            ((Math::Vector4D){d.x, d.y, d.z, 0}) * m.transpose();

        Math::Vector3D newDir = {temp.x, temp.y, temp.z};

        f32 sgnz = (newDir.z > 0) - (newDir.z < 0);
        f32 sgnx = (newDir.x > 0) - (newDir.x < 0);
        auto ret = ((Math::Point3D){sgnx / 2, 0, sgnz / 2} * m + mCenter).toPoint();
        return ret;
    }

    void Plane::computeBoundingBox()
    {
        mScale = {mLength, 0, mWidth};
        Math::Transform4D m = 
            Math::Util::scaleMatrix(mScale)*
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D(); 

        mBoundingBoxExtents.first = 
            ((Math::Point3D(-0.5f, 0.0f, -0.5f) * m) + mCenter).toPoint();

        mBoundingBoxExtents.second = 
            ((Math::Point3D(0.5f, 0.0f, 0.5f) * m) + mCenter).toPoint();
    }

    void Plane::computeBVH()
    {
        computeBoundingBox();
        mBvh = (BVH){this, nullptr, nullptr};
    }
}}}}

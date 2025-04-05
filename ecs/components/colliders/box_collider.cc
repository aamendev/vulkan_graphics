#include "box_collider.h"
#include "primitive_collisions.h"
namespace Lina { namespace ECS { namespace Components { namespace Colliders{

    Box::Box(std::string tag, const std::pair<Math::Point3D, Math::Point3D>& p)
    {
        initWithExtents(tag, p);
        mScale = {mLength, mWidth, mHeight};
    }

    void Box::initWithExtents(std::string tag, 
            const std::pair<Math::Point3D, Math::Point3D>& p)
    {
        mTag = tag;

        mCenter = p.first.midPoint(p.second);

        mLength = (mCenter - p.first).x * 2;
        mLength *= ((mLength >= 0) - (mLength < 0.0));

        mWidth = (mCenter - p.first).z * 2;
        mWidth *= ((mWidth >= 0) - (mWidth < 0.0));

        mHeight = (mCenter - p.first).y * 2;
        mHeight *= ((mHeight >= 0) - (mHeight < 0.0));
        mRotation = {0, 0, 0};

        mRecentCollisions = {};
        mCallDefaults = true;
        mGeometry = ColliderGeometry::Box;

        mCollisionEnterCallback = staticDefaultOnCollisionEnter;
        mResolveCallback = staticDefaultOnResolve;
        mCollisionExitCallback = staticDefaultOnCollisionExit;
        mCollisionPersistCallback = staticDefaultOnCollisionPersist;
    }
    b8 Box::checkCollision(Collider* c)
    {
        ColliderGeometry g = c->getColliderGeometry();
        switch (g)
        {
            case ColliderGeometry::Cylinder:
                return Helpers::Collisions::boxCylinderCollision(this, c);
                break;
            case ColliderGeometry::Plane:
                {
                    return Helpers::Collisions::boxPlaneCollision(this, c);
                }
                break;
            case ColliderGeometry::Mesh:
                return c->checkCollision(this);
                break;
            default: 
                {
                    return false;
                }
                break;
        }
    }

    Math::Point3D Box::furthestPoint(const Math::Vector3D &d)
    {
        mScale = {mLength, mWidth, mHeight};
        Math::Transform4D m = 
            Math::Util::scaleMatrix(mScale)*
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D(); 

        Math::Vector4D temp = 
            ((Math::Vector4D){d.x, d.y, d.z, 0}) * m.transpose();

        Math::Vector3D newDir = {temp.x, temp.y, temp.z};

        f32 sgnx = (newDir.x > 0) - (newDir.x < 0);
        f32 sgny = (newDir.y > 0) - (newDir.y < 0);
        f32 sgnz = (newDir.z > 0) - (newDir.z < 0);
        Math::Point3D ret;
            ret = (
                    (Math::Point3D){sgnx / 2, sgny / 2, sgnz / 2} 
                    * m
                    + mCenter).toPoint();
            return ret;
    }

    void Box::onCollisionEnter(Collider *c){
        //std::cerr << "Enter\n";
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void Box::onCollisionPersist(Collider *c)
    {

       // std::cerr << "Persist\n";
        mCollisionPersistCallback(this, c);
        if (mCallDefaults)
            defaultOnCollisionPersist(c);
    }

    void Box::onCollisionExit(Collider *c){
       // std::cerr << "Exi\n";
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionExit(c);
        }
    }
    void Box::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults)
            defaultOnResolve(c);
    }

    void Box::computeBoundingBox()
    {
        mScale = {mLength, mHeight, mWidth};
        Math::Transform4D m = 
            Math::Quatrenion::angleToQuat(mRotation).getRotationMatrix4D()*
        Math::Util::scaleMatrix(mScale);

        mBoundingBoxExtents.first = 
            ((Math::Point3D(-0.5f, -0.5f, -0.5f) * m) + mCenter).toPoint();

        mBoundingBoxExtents.second = 
            ((Math::Point3D(0.5f, 0.5f, 0.5f) * m) + mCenter).toPoint();
    }
    void Box::computeBVH()
    {
        computeBoundingBox();
        mBvh = (BVH){this, nullptr, nullptr};
    }
}}}}

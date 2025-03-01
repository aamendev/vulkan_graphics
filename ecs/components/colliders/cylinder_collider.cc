#include "cylinder_collider.h"
#include "collider.h"
#include "plane_collider.h"
namespace Lina { namespace ECS { namespace Components {
    b8 CylinderCollider::checkCollision(Collider* c)
    {
        ColliderGeometry g = c->getColliderGeometry();
        switch (g)
        {
            case ColliderGeometry::Cylinder:
                return cylinderCylinderCollision(dynamic_cast<CylinderCollider*>(c));
            case ColliderGeometry::Plane:
                {
                    return cylinderPlaneCollision(c);
                }
                break;
            default: 
                {
                         return false;
                     }
                     break;
        }
    }

    void CylinderCollider::onCollisionEnter(Collider *c){
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void CylinderCollider::onCollisionPersist(Collider *c)
    {
        mCollisionPersistCallback(this, c);
        if (mCallDefaults)
            defaultOnCollisionPersist(c);
    }

    void CylinderCollider::onCollisionExit(Collider *c){
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
            defaultOnCollisionExit(c);
        }
    }
    void CylinderCollider::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults)
            defaultOnResolve(c);
    }

    void CylinderCollider::defaultOnCollisionExit(Collider* c)
    {

    }

    void CylinderCollider::defaultOnCollisionPersist(Collider* c)
    {
        defaultOnCollisionEnter(c);
    }

    void CylinderCollider::defaultOnCollisionEnter(Collider* c)
    {
        switch(c->getColliderGeometry())
        {
            case ColliderGeometry::Cylinder:
                {
                    CylinderCollider* other = dynamic_cast<CylinderCollider*>(c);
                    Math::Vector3D otherCenter = other->getCenter();

                    Math::Vector3D proj1 = {mCenter.x, 0, mCenter.z};
                    Math::Vector3D proj2 = {otherCenter.x, 0, otherCenter.z};

                    f32 mag = (proj1 - proj2).magnitude();
                    f32 len = mRadius + other->getRadius() - mag;

                    if (mag == 0) {proj1 += {0.1, 0, 0};}
                    mCenter += ((proj1 - proj2).normalise() * len);
                    defaultOnResolve(c);
                }
                break;
            case ColliderGeometry::Plane:
                {
                    PlaneCollider* other = dynamic_cast<PlaneCollider*>(c);
                    Math::Vector3D otherCenter = other->getCenter();
                    mCenter += {0, (otherCenter.y - mCenter.y) - (mHeight / 2), 0};
                    defaultOnResolve(c);
                }
                break;
            default: {std::cerr <<"WULD\n";}
        }
    }

    void CylinderCollider::defaultOnResolve(Collider* c)
    {
        /*    switch(c->getColliderGeometry())
              {
              case ColliderGeometry::Cylinder:
              {
              std::cerr << "Current Center: " << mCenter << ", Other's Center" 
              << ((CylinderCollider*)c)->getCenter() << "\n";
              }
              break;
              case ColliderGeometry::Plane:
              {
              std::cerr << "Current Center: " <<mCenter << "Plane's Center"
              << ((PlaneCollider*)c)->getCenter() << "\n";
              }
              break;
              default: {std::cerr << "Just Resolved!\n";}*/
        //}
    }

    b8 CylinderCollider::cylinderPlaneCollision(Collider* c)
    {
        PlaneCollider* d = dynamic_cast<PlaneCollider*>(c);
        Math::Vector3D otherCenter = d->getCenter();


        b8 cond = mCenter.x > (otherCenter.x - d->getLength() / 2) && 
            mCenter.x < (otherCenter.x + d->getLength() / 2) &&
            mCenter.z > (otherCenter.z - d->getWidth() / 2) && 
            mCenter.z < (otherCenter.z + d->getWidth() / 2) &&
            otherCenter.y > (mCenter.y - mHeight / 2) && 
            otherCenter.y < (mCenter.y + mHeight / 2);

        return cond;
    }

    b8 CylinderCollider::cylinderCylinderCollision(CylinderCollider* c)
    {
        Math::Vector3D otherCenter = c->getCenter();
        Math::Vector3D proj1 = {mCenter.x, 0, mCenter.z};
        Math::Vector3D proj2 = {otherCenter.x, 0, otherCenter.z};

        b8 circleOverlap = (proj1 - proj2).squaredMagnitude() < 
            (mRadius + c->getRadius()) * (mRadius + c->getRadius());


        b8 sideOverlap = 
            ((mCenter.y - mHeight / 2) < 
             (c->getCenter().y + c->getHeight() / 2)) && 
            ((mCenter.y + mHeight / 2) > 
             (c->getCenter().y - c->getHeight() / 2));

        return circleOverlap && sideOverlap;
    }
}}}

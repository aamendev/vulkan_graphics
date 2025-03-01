#include "cylinder_collider.h"
#include "collider.h"
#include "plane_collider.h"
namespace Lina { namespace ECS { namespace Components {
    void PlaneCollider::onCollisionEnter(Collider *c){
        mCollisionEnterCallback(this, c);
        if (mCallDefaults)
        {
           // defaultOnCollisionEnter(c);
        }
        onResolve(c);
    }

    void PlaneCollider::onCollisionPersist(Collider *c)
    {
        mCollisionPersistCallback(this, c);
    }

    void PlaneCollider::onCollisionExit(Collider *c){
        mCollisionExitCallback(this, c);
        if (mCallDefaults)
        {
           // defaultOnCollisionExit(c);
        }
    }
    void PlaneCollider::onResolve(Collider *c) 
    {
        mResolveCallback(this, c);
        if (mCallDefaults){}
           // defaultOnResolve(c);
    }
}}}

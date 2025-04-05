#include "primitive_collisions.h"
#include "box_collider.h"
namespace Lina { namespace Helpers { namespace Collisions {
    b8 boxCylinderCollision(Collider* box, Collider* cyl)
    {
        Colliders::Box* b = (Colliders::Box*)box;
        Colliders::Cylinder* c = (Colliders::Cylinder*)cyl;
        
        auto pos = (b->getCenter() - c->getCenter());
        b8 yCheck = pos.y * pos.y > 
            ((b->getHeight() + c->getHeight()) * (b->getHeight() + c->getHeight()))/4;
        b8 xCheck = pos.x * pos.x > 
            ((b->getLength()/2 + c->getRadius()) * (b->getLength()/2 + c->getRadius()));
        b8 zCheck = pos.z * pos.z > 
            ((b->getWidth()/2 + c->getRadius()) * (b->getWidth()/2 + c->getRadius()));
        return !yCheck && !xCheck && !zCheck;
    }
    b8 planeCylinderCollision(Collider* plane, Collider* cyl)
    {
        return true;
    }
    b8 boxPlaneCollision(Collider* box, Collider* pl)
    {
        return true;
    }
}}}

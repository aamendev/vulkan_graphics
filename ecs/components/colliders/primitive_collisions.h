#ifndef PRIMITIVE_COLLISIONS_H
#define PRIMITIVE_COLLISIONS_H
#include "box_collider.h"
#include "colliders.h" 
#include "cylinder_collider.h"
#include "plane_collider.h"
namespace Lina { namespace Helpers { namespace Collisions {
    b8 boxCylinderCollision(Collider* box, Collider* cyl);
    b8 planeCylinderCollision(Collider* box, Collider* cyl);
    b8 boxPlaneCollision(Collider* box, Collider* pl);
    b8 planePlaneCollision(Collider* p1, Collider* p2);
    b8 cylinderCylinderCollision(Collider* c1, Collider* c2);
}}}
#endif

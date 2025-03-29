#ifndef PRIMITIVE_COLLISIONS_H
#define PRIMITIVE_COLLISIONS_H
#include "box_collider.h"
#include "colliders.h" 
#include "cylinder_collider.h"
#include "plane_collider.h"
namespace Lina { namespace Helpers { namespace Collisions {
    b8 boxCylinderCollision(Colliders::Box* box, Colliders::Cylinder* cyl);
    b8 planeCylinderCollision(Colliders::Box* box, Colliders::Cylinder* cyl);
    b8 boxPlaneCollision(Colliders::Box* box, Colliders::Plane* pl);
}}}
#endif

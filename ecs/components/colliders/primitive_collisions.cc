#include "primitive_collisions.h"
namespace Lina { namespace Helpers { namespace Collisions {
    b8 boxCylinderCollision(Colliders::Box* box, Colliders::Cylinder* cyl)
    {
        return true;
    }
    b8 planeCylinderCollision(Colliders::Box* box, Colliders::Cylinder* cyl)
    {
        return true;
    }
    b8 boxPlaneCollision(Colliders::Box* box, Colliders::Plane* pl)
    {
        return true;
    }
}}}

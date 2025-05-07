#include "primitive_collisions.h"
#include "box_collider.h"
#include "box_collider.h"
#include "cylinder_collider.h"
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
            
     //   return gjk(box, cyl);
        return !yCheck && !xCheck && !zCheck;
    }
    b8 planeCylinderCollision(Collider* plane, Collider* cyl)
    {
        Colliders::Plane* p = dynamic_cast<Colliders::Plane*>(plane);
        Colliders::Cylinder* c = dynamic_cast<Colliders::Cylinder*>(cyl);
        const Math::Vector3D& planeCenter = p->getCenter();
        const Math::Vector3D& cylCenter = c->getCenter();

        b8 cond = cylCenter.x > (planeCenter.x - p->getLength() / 2) && 
            cylCenter.x < (planeCenter.x + p->getLength() / 2) &&
            cylCenter.z > (planeCenter.z - p->getWidth() / 2) && 
            cylCenter.z < (planeCenter.z + p->getWidth() / 2) &&
            planeCenter.y > (cylCenter.y - c->getHeight() / 2) && 
            planeCenter.y < (cylCenter.y + c->getHeight() / 2);
        return cond;
    }
    b8 boxPlaneCollision(Collider* box, Collider* pl)
    {
        return true;
    }
    b8 planePlaneCollision(Collider* p1, Collider* p2)
    {
        return false;
    }
    b8 cylinderCylinderCollision(Collider* c1, Collider* c2)
    {
        Colliders::Cylinder* cyl1 = dynamic_cast<Colliders::Cylinder*>(c1);
        Colliders::Cylinder* cyl2 = dynamic_cast<Colliders::Cylinder*>(c2);
        const Math::Vector3D& firstCenter = cyl1->getCenter();
        const Math::Vector3D& secondCenter = cyl2->getCenter();
        Math::Vector3D proj1 = {firstCenter.x, 0, firstCenter.z};
        Math::Vector3D proj2 = {secondCenter.x, 0, secondCenter.z};

        b8 circleOverlap = (proj1 - proj2).squaredMagnitude() < 
            (cyl1->getRadius() + cyl2->getRadius()) * (cyl1->getRadius() + cyl2->getRadius());

        b8 sideOverlap = 
            ((firstCenter.y - cyl1->getHeight() / 2) < 
             (secondCenter.y + cyl2->getHeight() / 2)) && 
            ((firstCenter.y + cyl1->getHeight() / 2) > 
             (secondCenter.y - cyl2->getHeight() / 2));

        return circleOverlap && sideOverlap;
    }
}}}

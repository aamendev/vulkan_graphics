#ifndef VALIDATION_LAYER_H 
#define VALIDATION_LAYER_H
#include "layer.h"

#include "../events/key.h"
#include "../events/mouse.h"

#include "../ecs/components/colliders/cylinder_collider.h"
#include "../ecs/components/colliders/plane_collider.h"
#include "../ecs/systems/collision_system.h"
#include "../ecs/components/transform.h"
#include "../ecs/components/colliders/collider.h"
#include "../ecs/components/health.h"

#include "../core/timer.h"

#include "../physics/physics.h"
namespace Lina { namespace Graphics {
class ValidationLayer : public Layer
{
    public:
        virtual void run() override {};
        virtual void init() override;
        virtual void onKeyDown(Events::KeyPress& e) override {};
        virtual void onKeyUp(Events::KeyRelease& e) override {};
        virtual void onMouseMove(Events::MouseMove& m) override {};
    private:
        void collisionValidation();
        void transformValidation();
        void healthValidation();
        void timerValidation();
        void physicsValidation();
    private:
        void particleValidation();
    private:
        ECS::Components::CylinderCollider* c1; 
        ECS::Components::CylinderCollider* c2; 
        ECS::Components::PlaneCollider* p1;
        ECS::CollisionSystem cSystem;
        Core::Timer mTimer;
    private:
        ECS::Components::Transform trans1;
        ECS::Components::Health health1;
    private:
        Physics::Particle* p;    
};
}}
#endif

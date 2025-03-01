#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "system.h"
#include "../components/colliders/collider.h"

namespace Lina { namespace ECS {
    class CollisionSystem : public System
    {
        public:
            CollisionSystem(): mEnabled(true), mColliders({}) {}
            virtual void update() override;
            virtual void enable() override;
            virtual void disable() override;

        public:
            void addCollider(Components::Collider* c) {mColliders.push_back(c);};
        private:
            b8 mEnabled;
            std::vector<Components::Collider*> mColliders;
            
    };
}}
#endif

#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "system.h"
#include "../components/colliders/colliders.h"
#include <map>
#include <set>
#include <tuple>

namespace Lina { namespace ECS {
    class CollisionSystem : public System
    {
        public:
            CollisionSystem(CollisionOptimization opt = CollisionOptimization::Naive):
                mEnabled(true), mStaticColliders({}),
                mDynamicColliders({}), mBvhInit(false), mOptMode(opt), 
                mStaticRegistriy({})
                {
                }
        public:
            virtual void update() override;
            virtual void enable() override;
            virtual void disable() override;

        public:
            void addCollider(Components::Collider* c,
                    ColliderType t = ColliderType::Dynamic);

            void optimize();
            inline void deOptimize() {mOptMode = CollisionOptimization::Naive;}

        private:
            void naiveUpdate();
            void optimizedUpdate();
            void combineBVHs();
            void combineBVH(Components::Collider::BVH* b0, 
                    Components::Collider::BVH* b1);
            std::pair<std::set<std::string>,
                std::set<std::string>> checkCollideBvh(Components::Collider* c,
                        Components::Collider::BVH*);
        private:
            b8 mEnabled;
            b8 mBvhInit;
            CollisionOptimization mOptMode;
            std::vector<Components::Collider*> mStaticColliders;
            std::vector<Components::Collider*> mDynamicColliders;
            std::map<std::string, u32> mStaticRegistriy;
            Components::Collider::BVH mFullBvh;
            
    };
}}
#endif

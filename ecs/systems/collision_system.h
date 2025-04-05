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

            void constructGrid(Math::Point3D min, 
                    Math::Point3D max, u32 h);
        private:
            void naiveUpdate();
            void optimizedUpdate();
            void combineBVHs();
            void combineBVH(Components::Collider::BVH* b0, 
                    Components::Collider::BVH* b1);
            
            void checkCollideBvh(Components::Collider* c,
                        Components::Collider::BVH*, std::set<std::string>& coll,
                        std::set<std::string>& nCol);
            void updateGrid(u32 colliderIdx);
        private:
            b8 mEnabled;
            b8 mBvhInit;
            std::pair<Math::Point3D, Math::Point3D> mGridPos;
            u32 mGridSize;
            CollisionOptimization mOptMode;
            std::vector<Components::Collider*> mStaticColliders;
            std::vector<Components::Collider*> mDynamicColliders;
            std::map<std::string, u32> mStaticRegistriy;
            std::map<std::string, u32> mDynamicRegistry;
            std::map<std::string, std::set<u32>> mDynamicGrid;
            Components::Collider::BVH mFullBvh;
            std::vector<std::set<std::string>> mGrid;
            //std::vector<std::string> movedColliders;
            
    };
}}
#endif

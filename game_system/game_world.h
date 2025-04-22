#ifndef GAME_WORLD_H
#define GAME_WORLD_H
#include "../ecs/systems/system.h"
#include "../ecs/components/colliders/collider.h"
#include "../ecs/components/colliders/cylinder_collider.h"
#include "../ecs/components/colliders/plane_collider.h"
#include "../ecs/components/transform.h"
#include "../ecs/components/health.h"
#include "../ecs/components/material.h"
#include "../ecs/systems/collision_system.h"
#include "../ecs/systems/character_controller.h"
#include "../ecs/components/colliders/mesh_collider.h"
#include "../ecs/components/ai.h"
#include "../renderer.h"
#include "../shuttle.h"
#include "../core/core.h"
#include "game_object.h"
#include <unordered_map>
namespace Lina { namespace GameSystem {
    class GameWorld
    {
        public:
            GameWorld(): mCollisionSystems({}), mCharacterControllers({}), mObjects({}), 
                mHealthComponents({}), 
                mTransformComponents({}),
                mCylinderColliderComponents({}),
                mPlaneColliderComponents({}),
                mMeshColliderComponents({}),
                mRenderer(nullptr)
                {}

            virtual void preinit() = 0;
            virtual void init() = 0;
            virtual void onKeyDown(Events::KeyPress& e) = 0;
            virtual void onKeyUp(Events::KeyRelease& e) = 0;
            virtual void onMouseMove(Events::MouseMove& e) = 0;
            virtual void update() = 0;
            virtual void reset() = 0;

        public:
            void addObject(const GameObject& obj) {mObjects[obj.getTag()] = obj;}
            void removeObject(std::string tag);
            void addCollisionSystem(ECS::CollisionSystem* c) {mCollisionSystems.push_back(c);};
            void addCharacterController(ECS::CharacterController* c) {mCharacterControllers.push_back(c);};
            void setRenderer(Graphics::Renderer* r) {mRenderer = r;}

            void removeCollisionSystem() {mCollisionSystems.pop_back();}
            void removeCharacterController() {mCharacterControllers.pop_back();}

        public:
            std::unordered_map<std::string, ECS::Components::Health>&
                getHealthComponents() {return mHealthComponents;}

            std::unordered_map<std::string, ECS::Components::Transform>&
                getTrasforms() {return mTransformComponents;}
            std::unordered_map<std::string, ECS::Components::Material>&
                getMaterials() {return mMaterialComponents;}

        protected:
            void fetchComponents();
        protected:
            std::vector<ECS::CollisionSystem*> mCollisionSystems;
            std::vector<ECS::CharacterController*> mCharacterControllers;
            std::unordered_map<std::string, GameObject> mObjects;
            Graphics::Renderer* mRenderer;
            std::unordered_map<std::string, ECS::Components::Health> mHealthComponents;
            std::unordered_map<std::string, ECS::Components::Material> mMaterialComponents;
            std::unordered_map<std::string, ECS::Components::Transform> mTransformComponents;
            std::unordered_map<std::string, ECS::Components::Colliders::Cylinder> mCylinderColliderComponents;
            std::unordered_map<std::string, ECS::Components::Colliders::Plane> mPlaneColliderComponents;
            std::unordered_map<std::string, ECS::Components::Colliders::Mesh> mMeshColliderComponents;
            std::unordered_map<std::string, ECS::Components::AI> mAIComponents;
            Core::Timer mTimer;

    };
}}
#endif

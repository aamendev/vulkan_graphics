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
                mRenderer(nullptr)
                {}

            void preinit();
            void init();

            void addObject(const GameObject& obj) {mObjects[obj.getTag()] = obj;}
            void removeObject(std::string tag);
            void addCollisionSystem(ECS::CollisionSystem* c) {mCollisionSystems.push_back(c);};
            void addCharacterController(ECS::CharacterController* c) {mCharacterControllers.push_back(c);};
            void setRenderer(Graphics::Renderer* r) {mRenderer = r;}

            void onKeyDown(Events::KeyPress& e);
            void onKeyUp(Events::KeyRelease& e);
            void onMouseMove(Events::MouseMove& e){};
            void removeCollisionSystem() {mCollisionSystems.pop_back();}
            void removeCharacterController() {mCharacterControllers.pop_back();}
            void update();
            void reset();

        public:
            std::unordered_map<std::string, ECS::Components::Health>&
                getHealthComponents() {return mHealthComponents;}

            std::unordered_map<std::string, ECS::Components::Transform>&
                getTrasforms() {return mTransformComponents;}
            std::unordered_map<std::string, ECS::Components::Material>&
                getMaterials() {return mMaterialComponents;}

        private:
            void fetchComponents();
            void setCallBacks();
        private:
            std::vector<ECS::CollisionSystem*> mCollisionSystems;
            std::vector<ECS::CharacterController*> mCharacterControllers;
            std::unordered_map<std::string, GameObject> mObjects;
            Graphics::Renderer* mRenderer;
            Graphics::Shuttle mShuttle;
            std::unordered_map<std::string, ECS::Components::Health> mHealthComponents;
            std::unordered_map<std::string, ECS::Components::Material> mMaterialComponents;
            std::unordered_map<std::string, ECS::Components::Transform> mTransformComponents;
            std::unordered_map<std::string, ECS::Components::CylinderCollider> mCylinderColliderComponents;
            std::unordered_map<std::string, ECS::Components::PlaneCollider> mPlaneColliderComponents;

            Core::Timer mTimer;
            f32 mVelocity = 1.2f;
            f32 mJump = 1.2f;
            f32 mGravity = 0.020f;
            f32 mFrameRate = 60.0f;
            f32 mColliderOffset = 8.0f;
            u32 mCoinCount = 0;
            u32 mGoal = 2;
    };
}}
#endif

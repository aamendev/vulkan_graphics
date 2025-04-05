#ifndef PS_COLLISION_WORLD_H
#define PS_COLLISION_WORLD_H
#include "../game_system/game_world.h"
#include "../physics/physics.h"
#include <unordered_map>
namespace Lina { namespace Games {
    class CollisionWorld : public GameSystem::GameWorld
    {
        public:
            virtual void preinit() override;
            virtual void init() override;
            virtual void onKeyDown(Events::KeyPress& e) override;
            virtual void onKeyUp(Events::KeyRelease& e) override;
            virtual void onMouseMove(Events::MouseMove& e) override {};
            void setEnvVerts(std::vector<std::vector<f32>> verts) {mEnvVerts = verts;}
            void setCallBacks();

        public:
            virtual void update() override;
            virtual void reset() override;

        private:
            void addRamp(Math::Point3D&& p, Math::EulerAngles&& rot);
            void move();
            void addPSColliders();

        private:
            Physics::ParticleSystem mParticleSystem;
            std::vector<std::vector<f32>> mEnvVerts;
            Graphics::Shuttle mShuttle;
            f32 mFrameRate = 60.0f;
            f32 mZoom = 15.0f;
            f32 translationRate = 0.2f;
            Math::Vector3D mUp {0, 1, 0};
            Math::Vector3D mForward;
            Math::Vector3D mSide;
            Math::Vector3D slopeGravity;
            i32 xMove = 0;
            i32 zMove = 0;
            b8 isJump = false;
            b8 won =false;
            b8 onSlope = false;
            f32 mVelocity = 1.2f;
            f32 mJump = 0.5f;
            f32 mGravity = 0.020f;
            f32 mColliderOffset = 8.0f;
            u32 mCoinCount = 0;
            u32 mGoal = 2;
            u32 mRampCount = 0;
            u32 mCollectedCoins = 0;
            std::unordered_map<std::string, u32> mColMap;
    };
}}
#endif

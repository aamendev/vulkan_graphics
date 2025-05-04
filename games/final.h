#ifndef FINAL_GAME_H
#define FINAL_GAME_H
#include "../game_system/game_world.h"
#include "../physics/physics.h"
namespace Lina { namespace Games {
    class FinalGameWorld : public GameSystem::GameWorld
    {
        typedef struct LightData
        {
            Math::Vector4D ambient = {1.0f, 1.0f, 1.0f, 0.1f};
            Math::Vector4D lightPos = {0, -200, 0, 0};
            Math::Vector4D lightCol = {0.5f, 0.2f, 1.0f, 10000.9f};
        } LightData;
        typedef struct RayTraceData
        {
            Math::Vector4D rayOrg = {0, 0, 0, 0};
            Math::Vector4D rayDirection = {0, 0, 1, 0};
            Math::Vector4D rayUp = {0,1,0,0};
            Math::Vector4D sphereOrg = {0, 0, 0, 200.0f};
            f32 w;
            f32 h;
        } TraceData;
        public:
            inline void setEnvVerts(std::vector<f32> evns) {mEnvVerts = evns;}
            inline void setEnvInds(std::vector<u32> inds) {mEnvInds = inds;}
        public:
            virtual void preinit() override;
            virtual void init() override;
            virtual void onKeyDown(Events::KeyPress& e) override;
            virtual void onKeyUp(Events::KeyRelease& e) override;
            virtual void onMouseMove(Events::MouseMove& e) override {};

        public:
            virtual void update() override;
            virtual void reset() override;

        private:
            void setCallBacks();
            void move();
        private:
            Physics::SphericalShellParticleSystem mParticleSystem;
            Graphics::Shuttle mShuttle;
            std::vector<f32> mEnvVerts;
            std::vector<u32> mEnvInds;
            f32 mVelocity = 1.2f;
            f32 mJump = 200.0f;
            //f32 mGravity = 0.040f;
            f32 mFrameRate = 60.0f;
            f32 mGravity = 0.01f;

            f32 translationRate = 3.5f;
            u32 currentPlanet = 0;
            Math::Vector3D mUp {0, 1, 0};
            Math::Vector3D mForward;
            Math::Vector3D mSide;
            Math::Vector3D slopeGravity;
            i32 xMove = 0;
            i32 zMove = 0;
            b8 isJump = false;
            b8 onSlope = false;
            f32 ticks = 0.0f;
            b8 physicsMode = false;
            std::vector<
                std::array<ECS::Components::RGBA, 3>> planetLayers;

           Physics::RigidBody playerRb; 
           Physics::Particle sunParticle;
           std::vector<Physics::Force*> forces;
           LightData mLightData;
           TraceData mTraceData;
    };
}}
#endif

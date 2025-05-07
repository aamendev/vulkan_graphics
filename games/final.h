#ifndef FINAL_GAME_H
#define FINAL_GAME_H
#include "../game_system/game_world.h"
#include "../physics/physics.h"
#include "../geo_modifier.h"
namespace Lina { namespace Games {
    class FinalGameWorld : public GameSystem::GameWorld
    {
        typedef struct LightData
        {
            Math::Vector4D ambient = {1.0f, 1.0f, 1.0f, 0.0f};
            Math::Vector4D lightPos = {0, 0, 0, 50};
            Math::Vector4D lightCol = {1.0f, 1.0f, 1.0f, 1e5};
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
        typedef struct Planet
        {
            std::string tag = "";
            Physics::RigidBody* rb = nullptr;
            f32 rad = 20.0f;
            std::array<ECS::Components::RGBA, 3> colLayers 
                = {WHITE, WHITE, WHITE};
            b8 isStar = false;
        }Planet;
        public:
            inline void setEnvVerts(std::vector<f32> evns) {mEnvVerts = evns;}
            inline void setEnvInds(std::vector<u32> inds) {mEnvInds = inds;}
            inline void setEnvNorms(std::vector<Math::Vector3D> norms) {mEnvNorms = norms;}
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
            void addPlanet(Planet& p);
            //void addGravity(Planet& p1);
        private:
            u32 mPlanetCount = 0;
            u32 mStarCount = 0;
            Physics::SphericalShellParticleSystem mParticleSystem;
            Graphics::Shuttle mShuttle;
            std::array<Graphics::Shuttle, 2> mShuttles;
            std::vector<f32> mEnvVerts;
            std::vector<u32> mEnvInds;
            std::vector<Math::Vector3D> mEnvNorms;
            f32 mVelocity = 1.2f;
            f32 mJump = 200.0f;
            //f32 mGravity = 0.040f;
            f32 mFrameRate = 60.0f;
            f32 mGravity = 0.02f;

            i32 mFollowIdx = 0;;
            u32 mCurrentShuttle = 0;
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
            b8 physicsMode = true;
            b8 followMode = false;
            std::vector<
                std::array<ECS::Components::RGBA, 3>> planetLayers;

           //Physics::RigidBody playerRb; 
           //Physics::Particle sunParticle;
           LightData mLightData;
           TraceData mTraceData;
           Graphics::GeoModifier mGeoModifier;
           std::vector<Planet> mPlanets;
           u32 mCurrentPlanet = 0;
           f32 mG = 700.0f;
           f32 mZoom = 0.0f;
    };
}}
#endif

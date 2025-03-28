#ifndef PS_GAME_WORLD_H
#define PS_GAME_WORLD_H
#include "../game_system/game_world.h"
#include "../physics/physics.h"
namespace Lina { namespace Games {
    class PSWorld : public GameSystem::GameWorld
    {
        public:
            virtual void preinit() override;
            virtual void init() override;
            virtual void onKeyDown(Events::KeyPress& e) override;
            virtual void onKeyUp(Events::KeyRelease& e) override {};
            virtual void onMouseMove(Events::MouseMove& e) override;

        public:
            virtual void update() override;
            virtual void reset() override {};

        private:
            Graphics::Shuttle mShuttle;
            f32 mFrameRate = 60.0f;
            f32 mZoom = 15.0f;
            Physics::ParticleSystem mParticleSystem;
            Physics::Forces::Spring mSpring;
            Physics::Forces::Spring mSpring2;
            Physics::Forces::Gravity mGravity;
            std::vector<Physics::Particle> mParticles;
    };
}}
#endif

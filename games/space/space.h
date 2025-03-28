#ifndef SPACE_GAME_WORLD_H
#define SPACE_GAME_WORLD_H
#include "../../game_system/game_world.h"
#include "../../physics/physics.h"
namespace Lina { namespace Games {
    class Space : public GameSystem::GameWorld
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
            void addPlanet(f32 mass, b8 addOwnGravity, Math::Point3D&& pos, f32 r);
            void fetchGravity();

        private:
            Graphics::Shuttle mShuttle;
            f32 mFrameRate = 60.0f;
            std::vector<Physics::Forces::Gravity> mGravity;
            std::vector<Physics::Particle> mParticles;

            typedef struct color 
            {
                f32 r = 0.0f;
                f32 g = 1.0f;
                f32 b = 1.0f;
                f32 a = 1.0f;
            } Col;

            std::vector<Col> mColours;
            std::vector<f32> mScales;
            std::vector<b8> mHasGravity;
            u32 mGravityCount = 0;
    };
}}
#endif

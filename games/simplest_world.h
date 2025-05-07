#ifndef SIMPLEST_GAME_H
#define SIMPLEST_GAME_H
#include "../game_system/game_world.h"
#include "../physics/physics.h"
namespace Lina { namespace Games {
    class SimplestGameWorld : public GameSystem::GameWorld
    {
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
            Graphics::Shuttle mShuttle;
            std::vector<f32> mEnvVerts;
            std::vector<u32> mEnvInds;
            f32 mFrameRate = 60.0f;
            f32 translationRate = 0.01f;

            i32 xMove = 0;
            i32 zMove = 0;
            i32 yMove = 0;
            f32 zoom = 4.0f;
            f32 zoomChange = 0.10f;
    };
}}
#endif

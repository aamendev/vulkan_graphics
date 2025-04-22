#ifndef W2_GAME_WORLD_H 
#define W2_GAME_WORLD_H
#include "../game_system/game_world.h"
namespace Lina { namespace Games {
    class W2GameWorld : public GameSystem::GameWorld
    {
        public:
            virtual void preinit() override;
            virtual void init() override;
            virtual void onKeyDown(Events::KeyPress& e) override;
            virtual void onKeyUp(Events::KeyRelease& e) override;
            virtual void onMouseMove(Events::MouseMove& e) override {};

        public:
            virtual void update() override;
            virtual void reset() override;
            void setEnvVertes(std::vector<f32> vs) {mEnvVertices = vs;}

        private:
            void setCallBacks();
        private:
            Graphics::Shuttle mShuttle;
            std::vector<f32> mEnvVertices;
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

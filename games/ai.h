#ifndef AI_GAME_H
#define AI_GAME_H

#include "../game_system/game_world.h"
namespace Lina { namespace Games {
    class AiGameWorld : public GameSystem::GameWorld
    {
        public:
            inline void setGraphs(std::vector<Helpers::AI::Graph> gs)
            {mAiGraphs = gs;}
            inline void setEnvVerts(std::vector<f32> evns) {mEnvVerts = evns;}
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
            std::vector<Helpers::AI::Graph> mAiGraphs;
            std::vector<f32> mEnvVerts;
            f32 mVelocity = 1.2f;
            f32 mJump = 1.2f;
            f32 mGravity = 0.020f;
            f32 mFrameRate = 60.0f;

            f32 translationRate = 1.2f;
            Math::Vector3D mUp {0, 1, 0};
            Math::Vector3D mForward;
            Math::Vector3D mSide;
            Math::Vector3D slopeGravity;
            i32 xMove = 0;
            i32 zMove = 0;
            b8 isJump = false;
            b8 onSlope = false;
            u32 currentCollectible = 0;

    };
}}
#endif

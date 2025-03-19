#ifndef COLLISION_SIM_LAYER_H
#define COLLISION_SIM_LAYER_H
#include "../game_system/game_world.h"
#include "./layer.h"
#include "../Math/Matrix4D.h"
#include "../Math/Vector3D.h"
#include "../events/key.h"
#include "../events/mouse.h"
#include "../types.h"
#include "../renderer.h"
#include "../shapes/icosphere.h"
#include "../shuttle.h"
#include "../shapes/cylinder.h"
#include "../games/warmup2.h"
#include "../games/collision_world.h"
#include "../core/obj_loader.h"
#include <fstream>
#include <sstream>

namespace Lina { namespace Graphics {
    class CollisionSimLayer : public Layer
    {
        public:
            CollisionSimLayer(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
        public:
            void init() override;
            void run() override;
        public:
            void onMouseMove(Events::MouseMove& m) override;
            void onKeyDown(Events::KeyPress& k) override;
            void onKeyUp(Events::KeyRelease& k) override;
        private:
            Renderer* mRenderer;
            Window* mWindow;
            Games::CollisionWorld mGameWorld;
    };
}}
#endif

#ifndef SIMPLE_WORLD_LAYER_H
#define SIMPLE_WORLD_LAYER_H
#include "../games/simplest_world.h"
#include "../game_system/game_world.h"
#include "./layer.h"
#include "../modifier.h"
#include "../Math/Matrix4D.h"
#include "../Math/Vector3D.h"
#include "../events/key.h"
#include "../events/mouse.h"
#include "../types.h"
#include "../renderer.h"
#include "../shapes/icosphere.h"
#include "../shuttle.h"
#include "../shapes/cylinder.h"
#include "../games/final.h"
#include "../core/core.h"
#include "../geo_modifier.h"
#include "../color_modifier.h"
#include <fstream>
#include <sstream>

namespace Lina { namespace Graphics {
    class SimpleLayer : public Layer
    {
        public:
            SimpleLayer(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
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
            Games::SimplestGameWorld  mGameWorld;
            GeoModifier mGeoModifier;
            ColModifier mColModifier;
    };
}}
#endif

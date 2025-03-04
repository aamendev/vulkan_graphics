#ifndef TEST_SCENE_LAYER_H
#define TEST_SCENE_LAYER_H
#include "./layer.h"
#include "../Math/Matrix4D.h"
#include "../Math/Vector3D.h"
#include "../events/key.h"
#include "../events/mouse.h"
#include "../types.h"
#include "../renderer.h"
#include "../shapes/icosphere.h"
#include "../shuttle.h"
#include <fstream>
#include <sstream>
namespace Lina { namespace Graphics{
    class TestSceneLayer : public Layer
    {
        public:
            TestSceneLayer(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
        public:
            void init() override;
            void run() override;
        public:
            void onMouseMove(Events::MouseMove& m) override{};
            void onKeyDown(Events::KeyPress& k) override;
            void onKeyUp(Events::KeyRelease& k) override {}
        private:
            Renderer* mRenderer;
            Window* mWindow;
    };
}}
#endif

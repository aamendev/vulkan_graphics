#ifndef SCENE_LAYER_H
#define SCENE_LAYER_H
#include "./layer.h"
#include "../Math/Matrix4D.h"
#include "../Math/Vector3D.h"
#include "../events/key.h"
#include "../events/mouse.h"
#include "../Types.h"
#include "../renderer.h"
#include "../shapes/icosphere.h"
#include "../shuttle.h"
#include <fstream>
#include <sstream>
namespace Lina { namespace Graphics{
    class SceneLayer : public Layer
    {
        public:
            SceneLayer(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
        public:
            void init() override;
            void run() override;
        public:
            void onMouseMove(Events::MouseMove& m) override{};
            void onKeyDown(Events::KeyPress& k) override;
        private:
            Math::Matrix4D fulltransMat;
            std::vector<std::pair<std::string, b8>> texData;
            std::vector<Lina::Math::Vector3D> pos;
            std::vector<f32> radii;
            std::vector<f32> orbital_speeds;
            std::vector<f32> selforbital_speeds;
            std::fstream fin;
            std::string line;
            std::string temp;
            std::string word;
            std::vector<std::string> data;
            std::vector<Lina::Math::Transform4D> currTransforms;
            int followIndex= 0;
            int shuttleIndex = 0;
            std::vector<Shuttle> shuttles;
            Renderer* mRenderer;
            Window* mWindow;
    };
}}
#endif

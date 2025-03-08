#ifndef PS_LAYER_H
#define PS_LAYER_H
#include "../games/ps.h"
#include "layer.h"
#include "../shapes/shapes.h"
namespace Lina { namespace Graphics {
    class PSLayer : public Layer
    {
        public:
            PSLayer(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
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
            Games::PSWorld  mGameWorld;
    };
}}
#endif

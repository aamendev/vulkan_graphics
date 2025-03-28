#ifndef SPACE_H
#define SPACE_H
#include "../layer.h"
#include "../../shapes/shapes.h"
#include "../../games/space/space.h"
namespace Lina { namespace Graphics {
    class Space : public Layer
    {
        public:
            Space(Renderer* r, Window* w): mWindow(w), mRenderer(r){}
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
            Games::Space  mGameWorld;
    };
}}
#endif

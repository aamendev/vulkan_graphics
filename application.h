#pragma once
#include "common.h"
#include "window.h"
#include "renderer.h"
namespace Lina{
    class App
    {
        public:
            void run();
            void drawFrame();
        private:
            void initWindow();
            void initRenderer();
            void mainLoop();
            void cleanUp();
        private:
            Graphics::Window* mWindow;
            Graphics::Renderer* mRenderer;
    };
}

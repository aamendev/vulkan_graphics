#pragma once
#include "common.h"
#include "window.h"
#include "renderer.h"
namespace Lina{
    class App
    {
        public:
            void run();
            void drawFrame(void* data = nullptr);
        private:
            void initWindow();
            void initRenderer();
            void mainLoop();
            void cleanUp();
            void preprocess();
        private:
            Graphics::Window* mWindow;
            Graphics::Renderer* mRenderer;
    };
}

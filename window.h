#pragma once
#include "common.h"
#include <GLFW/glfw3.h>
namespace Lina{ namespace Graphics{
    class Window
    {
        public:
            struct specs
            {
                const char* name;
                f32 width;
                f32 height;
                b8 resizable = false;
                b8 resized = false;
            };
            Window(specs& s);
            ~Window();
            b8 Create(specs& s);
            std::string getName();
            float getWidth() {return mSpecs.width;}
            float getHeight() {return mSpecs.height;}
            b8 isClosed();
            void update();
            b8 createWindowSurface(VkInstance& instance, VkSurfaceKHR* surface);
            GLFWwindow* getWindow() {return mWindow;}
            b8 isResized();
            void setResized(bool val) {mSpecs.resized = val;}
        private:
            b8 init();
            void clear();
            void setColour();
        private:
            specs mSpecs;
            GLFWwindow* mWindow;

    };
}}

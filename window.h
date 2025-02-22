#pragma once
#include "common.h"
#include <GLFW/glfw3.h>
#include "events/events.h"
namespace Lina{ namespace Graphics{
    class Window
    {
        public:
            struct specs
            {
                const char* name;
                f64 width;
                f64 height;
                b8 resizable = false;
                b8 resized = false;
                std::vector<Events::Event*> mEvents;
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
            std::vector<Events::Event*>& getEvents() {return mSpecs.mEvents;}
            void clearEvents() {mSpecs.mEvents.clear();}
        private:
            b8 init();
            void clear();
            void setColour();
            void registerEventCallbacks();
        private:
            specs mSpecs;
            GLFWwindow* mWindow;
    };
}}

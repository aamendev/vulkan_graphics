#pragma once
#include "common.h"
#include "events/events.h"
#include "./layers/layer.h"
#include "./layers//scene_layer.h"
#include "window.h"
#include "renderer.h"
#include "shuttle.h"
namespace Lina{
    class App
    {
        public:
            App(): mCurrentLayer(0){init();}
            void run();
            void drawFrame(void* data = nullptr);

        template <typename EventType, typename Callback>  
            void catSub(Callback f) 
            {
                for (int i = 0; i < mEvents.size(); i++)
                {
                    auto* e = mEvents[i];
                    if (e->getType() == EventType::getStaticType())
                    {
                        e->subscribe(f, static_cast<EventType&>(*e));
                    }
                }
            };
        private:
            void init();
            void initWindow();
            void initRenderer();
            void mainLoop();
            void cleanUp();
            void preprocess();
            void handleEvents();
        private:
            Graphics::Window* mWindow;
            Graphics::Renderer* mRenderer;
            std::vector<Events::Event*> mEvents;
            std::vector<Graphics::Layer*> mLayers;
            int mCurrentLayer;
    };
}

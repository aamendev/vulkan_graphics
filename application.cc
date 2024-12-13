#include "application.h"
#include "events/events.h"
#include "events/key.h"
#include "layers/test_scene.h"
#include <string>

namespace Lina{
    void App::run()
    {
        mainLoop(); 
    }

    void App::init()
    {
        Graphics::Window::specs s= {
            .name="Lina Main Screen",
            .width = 700,
            .height = 600,
            .resizable = true,
            .mEvents = {}
        };
        mWindow = new Graphics::Window(s);

        mRenderer = new Graphics::Renderer();
        std::string mainName = "Lina Main Application";
        mRenderer->init(mainName, mWindow);

        Graphics::SceneLayer* mainSceneLayer = 
            new Graphics::SceneLayer(mRenderer, mWindow);

        Graphics::TestSceneLayer* testLayer = 
            new Graphics::TestSceneLayer(mRenderer, mWindow);

        mLayers.push_back(mainSceneLayer);
        mLayers.push_back(testLayer);
        mCurrentLayer = 0;
        for (int i = 0; i < mLayers.size(); i++) mLayers[i]->init();
    }

    void App::handleEvents()
    {
        for (auto* e : mEvents)
        {
            e->handle();
            delete e;
        }
            mWindow->clearEvents();
            mEvents.clear();
    }

    void App::mainLoop()
    {

        while(!mWindow->isClosed())
        { 
            mLayers[mCurrentLayer]->run();
            mWindow->update();

            mEvents = mWindow->getEvents();

            std::function<void(Events::KeyPress&)> f1 = 
                std::bind(
                        &Graphics::Layer::onKeyDown, 
                        mLayers[mCurrentLayer], 
                        std::placeholders::_1);

            catSub<Events::KeyPress>(f1);
            handleEvents();
        }
    }
}

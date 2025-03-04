#include "application.h"
#include "events/events.h"
#include "events/key.h"
#include "layers/test_scene.h"
#include "layers/warmup2_layer.h"
#include "layers/validation_layer.h"
#include "types.h"
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
        Graphics::WarmUp2Layer* warmup = new Graphics::WarmUp2Layer(mRenderer, mWindow);

        Graphics::TestSceneLayer* testLayer = 
            new Graphics::TestSceneLayer(mRenderer, mWindow);

        Graphics::ValidationLayer* validLayer = new Graphics::ValidationLayer();

        mLayers.push_back(mainSceneLayer);
        mLayers.push_back(testLayer);
       mLayers.push_back(warmup);
     //   mLayers.push_back(validLayer);

        mCurrentLayer = 0;
        for (int i = 0; i < mLayers.size(); i++) mLayers[i]->init();

        mRenderer->createGraphicsPipelines();
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

    void App::onKeyDown(Events::KeyPress& e)
    {
        switch(e.key())
        {
            case Input::KeyCode::N:
            {
                mCurrentLayer = (mCurrentLayer + 1) % mLayers.size();
            }
            break;
            default:{}
        }
    }
    void App::mainLoop()
    {

       while(!mWindow->isClosed())
        { 
            mEvents = mWindow->getEvents();
            std::function<void(Events::KeyPress&)> f1 = 
                std::bind(
                        &Graphics::Layer::onKeyDown, 
                        mLayers[mCurrentLayer], 
                        std::placeholders::_1);

               std::function<void(Events::KeyPress&)> appDown = 
                 std::bind(
                 &App::onKeyDown, 
                 this, 
                 std::placeholders::_1);

            std::function<void(Events::KeyRelease&)> f2 = 
                std::bind(
                        &Graphics::Layer::onKeyUp, 
                        mLayers[mCurrentLayer], 
                        std::placeholders::_1);

            catSub<Events::KeyPress>(f1);
            catSub<Events::KeyPress>(appDown);
            catSub<Events::KeyRelease>(f2);

            handleEvents();

            mLayers[mCurrentLayer]->run();
            mWindow->update();


        }
    }
}

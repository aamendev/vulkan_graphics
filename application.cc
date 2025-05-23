#include "application.h"
#include "events/events.h"
#include "events/key.h"
#include "layers/ai_layer.h"
#include "layers/collision_sim.h"
#include "layers/test_scene.h"
#include "layers/warmup2_layer.h"
#include "layers/validation_layer.h"
#include "layers/simple.h"
#include "layers/ps_layer.h"
#include "layers/space/space.h"
#include "layers/final_layer.h"
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
            .width = 800,
            .height = 800,
            .resizable = false,
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
        Graphics::PSLayer* psLayer = new Graphics::PSLayer(mRenderer, mWindow);

        Graphics::ValidationLayer* validLayer = new Graphics::ValidationLayer();

        Graphics::CollisionSimLayer* simLayer = 
            new Graphics::CollisionSimLayer(mRenderer, mWindow);

        Graphics::WarmUp2Layer* warmup = new Graphics::WarmUp2Layer(mRenderer, mWindow);
        Graphics::AiLayer* ai = new Graphics::AiLayer(mRenderer, mWindow);
        Graphics::FinalLayer* final = new Graphics::FinalLayer(mRenderer, mWindow);
        Graphics::Space* spaceLayer = new Graphics::Space(mRenderer, mWindow);
        Graphics::SimpleLayer* simpleLayer = new Graphics::SimpleLayer(mRenderer, mWindow);
        //mLayers.push_back(mainSceneLayer);
        //mLayers.push_back(testLayer);
//      mLayers.push_back(warmup);
//        mLayers.push_back(validLayer);
        //mLayers.push_back(psLayer);
//        mLayers.push_back(simLayer);
        //mLayers.push_back(ai);
        mLayers.push_back(final);
        //mLayers.push_back(simpleLayer);
        //mBackgroundLayers.push_back(validLayer);
//        mLayers.push_back(spaceLayer);
        mCurrentLayer = 0;
        for (int j = 0; j < mBackgroundLayers.size(); j++)
        {
            mBackgroundLayers[j]->init();
        }
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

            std::function<void(Events::MouseMove&)> m1 = 
                std::bind(
                        &Graphics::Layer::onMouseMove, 
                        mLayers[mCurrentLayer], 
                        std::placeholders::_1);

            catSub<Events::KeyPress>(f1);
            catSub<Events::KeyPress>(appDown);
            catSub<Events::KeyRelease>(f2);
            catSub<Events::MouseMove>(m1);

            handleEvents();

            mLayers[mCurrentLayer]->run();
            mWindow->update();

        }
       for (int i = 0; i < mLayers.size(); i++)
       {
            delete(mLayers[i]);
       }
    }
}

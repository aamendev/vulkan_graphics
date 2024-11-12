#include "application.h"
#include "./Math/Utilities.h"
#include "./Math/Matrix3D.h"
#include "./Math/Vector4D.h"
#include "./shapes/icosphere.h"
#include "./shuttle.h"
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Types.h"
#include "events/events.h"
#include "events/key.h"
#include <fstream>
#include <sstream>
#include <string>

Lina::Math::Matrix4D fulltransMat;
std::vector<std::pair<std::string, b8>> texData;
std::vector<Lina::Math::Vector3D> pos;
std::vector<f32> radii;
std::vector<f32> orbital_speeds;
std::vector<f32> selforbital_speeds;
std::fstream fin;
std::string line;
std::string temp;
std::string word;
std::vector<std::string> data;
std::vector<Lina::Math::Transform4D> currTransforms;
int followIndex= 0;
namespace Lina{
    void App::run()
    {
        initWindow();
        initRenderer();
        mainLoop(); 
//        cleanUp();
    }

    void App::initWindow()
    {

        Graphics::Window::specs s= {
            .name="Lina Main Screen",
            .width = 700,
            .height = 600,
            .resizable = false,
                .mEvents = {}
        };
        mWindow = new Graphics::Window(s);
    }

    void App::preprocess()
    {
        fin.open("../assets/planets.csv", std::ios::in);
        fin >> temp; //reomve header
        float damp = 0.06f;
        while (fin >> temp)
        {
            data.clear();
            std::stringstream s(temp);
            while (getline(s, word, ','))
            {
                data.push_back(word);
            }

            pos.emplace_back(stof(data[1]), stof(data[2]), stof(data[3]));
            radii.emplace_back(stof(data[4]));
            orbital_speeds.emplace_back(damp * stof(data[5]));
            selforbital_speeds.emplace_back(damp * 2 * stof(data[6]));
            texData.emplace_back(std::string("../assets/") + data[7], true);
            currTransforms.emplace_back(1.0f, 0, 0, stof(data[1]),
                    0, 1, 0.0f, stof(data[2]),
                    0, 0, 1.0f, stof(data[3]));
        }
        fin.close();
        mRenderer->createTexture(texData);
        std::vector<float> vertices =
        {
            -0.5f, -0.5f, 0.1f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.1f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
            -0.5f, 0.5f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

            -0.5f, -0.5f, 0.3f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
            -0.5f, 0.5f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
        };

        Graphics::Shapes::Icosphere ico(3.0f);
        ico.subdivide(5);
        auto tex = ico.getTextureDebug();
        auto verts = ico.getFullVertices();
        auto ind = ico.getIndices();
        std::vector<u32> indices =
        {
            0, 1, 2, 2, 3, 0,
            6, 5, 4, 4, 7, 6,
        };

        Graphics::VertexBufferLayout layout; 

        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT3, 1);
        layout.push(Graphics::Format::FLOAT2, 2);

          
        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);
        mRenderer->createGraphicsPipeline();
        mRenderer->createUniformBuffers(sizeof(fulltransMat));
    }
    void App::initRenderer()
    {
        mRenderer = new Graphics::Renderer();
        std::string mainName = "Lina Main Application";
        mRenderer->init(mainName, mWindow);
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
    b8 updateFollow(Events::KeyPress& e)
    {
        auto k = e.key();

        switch (k) {
            case Input::KeyCode::K:
                followIndex = (followIndex + 1) % (9);
                break;
            case Input::KeyCode::L:
                if (followIndex < 1) {followIndex = 9;}
                followIndex--;
                break;
                default:{}
        }
        return true;
    }
    void App::mainLoop()
    {
        preprocess();

        Graphics::Shuttle shuttle;
        //shuttle.setPosition(0.0, 0.0, -50.0);
        float theta = 0.0f;
        float speed = 4.0f;
        int i = 0;
        auto proj = Math::Util::projMatrix(135, mWindow->getWidth() / mWindow-> getHeight(), true);
        while(!mWindow->isClosed())
            //while (i++ < 1)
        { 
            mRenderer->beginDraw();
            mRenderer->setPrimitive(Primitive::Triangle);
            //followIndex = 3;
            for (int i = 0; i < 9; i++)
            {
                auto followTransform = currTransforms[followIndex].getTranslation();
                shuttle.setPosition(followTransform.x, followTransform.y, followTransform.z - 15*radii[followIndex]);
                fulltransMat = 
                    proj *
                    shuttle.getMatrix()*
                    currTransforms[i] *
                    Math::Util::scaleMatrix({radii[i], radii[i], radii[i]});
                mRenderer->updateUniform(&fulltransMat);
                mRenderer->render(nullptr, nullptr, i);

                auto curr = currTransforms[i].getTranslation();
                auto c = currTransforms[0].getTranslation();
                Math::Transform4D rotMat =
                    Math::Util::rotationMatrix4D(selforbital_speeds[i], Math::Util::yAxis())
                    * Math::Util::translationMatrix(c - curr)
                    * Math::Util::rotationMatrix4D(orbital_speeds[i], Math::Util::yAxis())
                    * Math::Util::translationMatrix(curr - c);

                currTransforms[i] = rotMat * currTransforms[i];
            }
            mRenderer->endDraw();
            mWindow->update();

            mEvents = mWindow->getEvents();
            std::function<bool(Events::KeyPress&)> f1 = std::bind(
                    &Graphics::Shuttle::update, &shuttle, std::placeholders::_1);
            catSub<Events::KeyPress>(f1);
            catSub<Events::KeyPress>(std::function(updateFollow));
            handleEvents();
        }
        
        
    }
}

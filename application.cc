#include "application.h"
#include "./Math/Utilities.h"
#include "./Math/Transform4D.h"
#include "./Math/Matrix3D.h"
#include "./Math/Point3D.h"
#include "./Math/Vector4D.h"
namespace Lina{
    void App::run()
    {
        initWindow();
        initRenderer();
        mainLoop();
        //cleanUp();
    }

    void App::initWindow()
    {

        Graphics::Window::specs s= {
            .name="Lina Main Screen",
            .width = 720,
            .height = 720,
            .resizable = false
        };
        mWindow = new Graphics::Window(s);
    }

    void App::initRenderer()
    {
       mRenderer = new Graphics::Renderer();
       std::string mainName = "Lina Main Application";
       mRenderer->init(mainName, mWindow);
       mRenderer->createTexture("../assets/psycopass1.jpg");
    }

    void App::mainLoop()
    {
        while(!mWindow->isClosed())
        {
          mWindow->update();
          drawFrame();
        }
    }

    void App::drawFrame()
    {
        std::vector<float> vertices =
        {
            -0.5f, -0.5f, 0.0f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
            -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

            -0.5f, -0.5f, -0.3f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,
            0.5f, -0.5f, -0.3f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.3f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
            -0.5f, 0.5f, -0.3f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f
        };
        Lina::Math::Point3D pos = {0, 0.3,2.0};
        Lina::Math::Matrix3D rotMat =
            Lina::Math::Util::rotationMatrix(20, Lina::Math::Util::xAxis());
         Lina::Math::Transform4D transMat =
            Lina::Math::Util::transMatrix(rotMat, pos);
         Lina::Math::Matrix4D  fulltransMat =
             Lina::Math::Util::projMatrix(
                     90.0f, mWindow->getWidth() / mWindow->getHeight()) *
             transMat *
             Lina::Math::Util::scaleMatrix({1, 1, 1.0});
        std::vector<u32> indices =
        {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4
        };
        Lina::Graphics::VertexBufferLayout layout;

        layout.push(Lina::Graphics::Format::FLOAT3, 0);
        layout.push(Lina::Graphics::Format::FLOAT3, 1);
        layout.push(Lina::Graphics::Format::FLOAT2, 2);

        mRenderer->createGraphicsPipeline();
        mRenderer->createVertexBuffer(layout, vertices);
        mRenderer->createIndexBuffer(indices);
        mRenderer->createUniformBuffers(sizeof(fulltransMat));

        mRenderer->updateUniform(&fulltransMat);

        mRenderer->beginDraw();
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->endDraw();
    }
}

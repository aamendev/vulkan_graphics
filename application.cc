#include "application.h"
#include "./Math/Utilities.h"
#include "./Math/Matrix3D.h"
#include "./Math/Vector4D.h"
#include "index_buffer.h"
#include "vertex_buffer.h"
Lina::Math::Matrix4D fulltransMat;
Lina::Graphics::VertexBuffer vb1;
Lina::Graphics::VertexBuffer vb2;
Lina::Graphics::IndexBuffer ib1;
Lina::Graphics::IndexBuffer ib2;
namespace Lina{
    void App::run()
    {
        initWindow();
        initRenderer();
        mainLoop(); //cleanUp();
    }

    void App::initWindow()
    {

        Graphics::Window::specs s= {
            .name="Lina Main Screen",
            .width = 700,
            .height = 600,
            .resizable = true
        };
        mWindow = new Graphics::Window(s);
    }

    void App::preprocess()
    {
        mRenderer->createTexture("../assets/psycopass1.jpg");
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

        std::vector<float> vertices2 =
        {
            -0.8f, -0.5f, 0.5f, 1.0f, 1.0f,1.0f, 1.0f, 0.0f,
            0.3f, -0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.3f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,  0.0f, 1.0f,
            -0.8f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        };
        std::vector<u32> indices =
        {
            0, 1, 2, 2, 3, 0,
            6, 5, 4, 4, 7, 6,
        };

        std::vector<u32> indices2 =
        {
            0, 1, 2, 2, 3, 0,
        };

        Graphics::VertexBufferLayout layout; 

        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT3, 1);
        layout.push(Graphics::Format::FLOAT2, 2);
          
        mRenderer->createVertexBuffer(layout, vertices);
        vb1 = mRenderer->getSpecs()->vertexBuffer;
        mRenderer->createVertexBuffer(layout, vertices2);
        vb2 = mRenderer->getSpecs()->vertexBuffer;
        mRenderer->createIndexBuffer(indices);
        ib1 = mRenderer->getSpecs()->indexBuffer;
        mRenderer->createIndexBuffer(indices);
        ib2 = mRenderer->getSpecs()->indexBuffer;
        mRenderer->createGraphicsPipeline();
        mRenderer->createUniformBuffers(sizeof(fulltransMat));


    }
    void App::initRenderer()
    {
        mRenderer = new Graphics::Renderer();
        std::string mainName = "Lina Main Application";
        mRenderer->init(mainName, mWindow);
    }

    void App::mainLoop()
    {
        preprocess();
        float theta = 0.0f;
        float speed = 6.0f;
        int i = 0;
        auto proj = Math::Util::projMatrix(90, mWindow->getWidth() / mWindow-> getHeight());
        while(!mWindow->isClosed())
        //while (i++ < 1)
        { 
            theta += (0.01 * speed) * (theta < 360.0f) + (-theta) * !(theta < 360.0f);

            mRenderer->beginDraw();
            mRenderer->setPrimitive(Primitive::Triangle);

            Math::Matrix3D rotMat =
                Math::Util::rotationMatrix(theta * 3.1415 / 180, Math::Util::zAxis());

            fulltransMat = 
                proj *
                Math::Util::transMatrix(rotMat, {0,0.3,2.0}) * Math::Util::scaleMatrix({2.0, 2.0, 2.0});
            mRenderer->updateUniform(&fulltransMat);
            mRenderer->render();

             rotMat =
                Math::Util::rotationMatrix(theta * 3.1415 / 180, Math::Util::xAxis());
            fulltransMat = 
                proj *
                Math::Util::transMatrix(rotMat, {0,0.3,2.0}) * Math::Util::scaleMatrix({2.0, 2.0, 2.0});

            mRenderer->updateUniform(&fulltransMat);
            mRenderer->render(&vb1, &ib1);

            mRenderer->endDraw();
            mWindow->update();
        }
    }
}

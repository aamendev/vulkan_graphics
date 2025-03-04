#include "./test_scene.h"

namespace Lina{ namespace Graphics{
    void TestSceneLayer::onKeyDown(Events::KeyPress& e)
    {

    }

    void TestSceneLayer::init()
    {
        Graphics::Shapes::Icosphere ico(3.0f);
        ico.subdivide(5);

        auto verts = ico.getFullVertices();
        auto ind = ico.getIndices();

        Graphics::VertexBufferLayout layout; 

        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT4, 1);


        mRenderer->toggleValidation();
        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);

        mRenderer->addShader("../shaders/compiled/simple.vert.spv", 
                "../shaders/compiled/simple.frag.spv");

//        mRenderer->createGraphicsPipelines();
    }
    void TestSceneLayer::run()
    {
        auto proj = Math::Util::projMatrix(135, mWindow->getWidth() / mWindow-> getHeight(), true);
        mRenderer->beginDraw();
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->endDraw();
    }
}}

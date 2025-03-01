#include "warmup2_layer.h"
namespace Lina{ namespace Graphics{
    void WarmUp2Layer::init()
    {

        // Renderer
        Graphics::Shapes::Cylinder cyl(0.5, 1, 20, 
                MeshMode::Pos3Col3, {.col = {1.0f, 0.2f, 0.3f}});

        auto verts = cyl.getFullVertices();
        auto ind = cyl.getIndices();

        std::vector<float> verts2 = {
            -0.5, 0, 0.5, 0, 0, 1,
            0.5, 0, 0.5, 1, 0, 0,
            -0.5, 0, -0.5, 0, 1, 0,
            0.5, 0, -0.5, 1, 0, 0,
        };

        std::vector<u32> ind2 = {2, 1, 0, 2, 3, 1};

        Graphics::VertexBufferLayout layout; 
        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT3, 1);

        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);
        mRenderer->createVertexBuffer(layout, verts2);
        mRenderer->createIndexBuffer(ind2);

        Shader defaultShader;
        Shader defaultShader2;

        Uniform color = 
        {
            .name = "col",
            .size = sizeof(f32) * 4,
            .binding = 0,
            .stage = ShaderStage::Fragment
        };

        PushConstant mvp = 
        {
            .stage = ShaderStage::Vertex, 
            .offset = 0,
            .size = sizeof(Math::Transform4D),
        };

        defaultShader.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");

        defaultShader2.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");

        defaultShader.addUniform(color);

        defaultShader2.addUniform(color);

        defaultShader.addPushConstant(mvp);
        defaultShader2.addPushConstant(mvp);

        mRenderer->addShader(defaultShader); 
        mRenderer->addShader(defaultShader2); 
        mRenderer->createGraphicsPipelines();
        
        // Game World
        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void WarmUp2Layer::run()
    {
        mGameWorld.update();
    }


    void WarmUp2Layer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void WarmUp2Layer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void WarmUp2Layer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}

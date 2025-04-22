#include "warmup2_layer.h"
#include "../core/tiny_based_obj_loader.h"
namespace Lina{ namespace Graphics{
    void WarmUp2Layer::init()
    {
        // Renderer
        Graphics::Shapes::Cylinder cyl(0.5, 1, 20, 
                MeshMode::Pos3, {.col = {1.0f, 0.2f, 0.3f}});

        auto verts = cyl.getFullVertices();
        auto ind = cyl.getIndices();

        /*std::vector<float> verts2 = {
            -0.5, 0, 0.5,
            0.5, 0, 0.5,
            -0.5, 0, -0.5,
            0.5, 0, -0.5,
        };*/
        Core::TinyBasedObjLoader obj;
        obj.load("../assets/env.obj");

        auto verts2 = obj.getPositions();
        auto ind2 = obj.getIndices();

        //std::vector<u32> ind2 = {2, 1, 0, 2, 3, 1};

        Graphics::VertexBufferLayout layout; 
        layout.push(Graphics::Format::FLOAT3, 0);
        //layout.push(Graphics::Format::FLOAT3, 1);

        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);

        mRenderer->createVertexBuffer(layout, verts2);
        mRenderer->createIndexBuffer(ind2);

        Shader defaultShader;
        Shader defaultShader2;

        Uniform color = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Dynamic,
            .size = sizeof(f32) * 4,
            .count = 2,
        };

        Uniform color2 = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Static,
            .size = sizeof(f32) * 4,
            .count = 1,
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

        defaultShader2.addUniform(color2);

        defaultShader.addPushConstant(mvp);
        defaultShader2.addPushConstant(mvp);

        mRenderer->addShader(defaultShader); 
        mRenderer->addShader(defaultShader2); 
        
        // Game World
        mGameWorld.setEnvVertes(verts2);
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

#include "dynamic_layer.h"
namespace Lina{ namespace Graphics{
    void DynamicCollisionLayer::init()
    {
        // Renderer
        Uniform color = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .size = sizeof(f32) * 4,
        };

        PushConstant mvp = 
        {
            .stage = ShaderStage::Vertex, 
            .offset = 0,
            .size = sizeof(Math::Transform4D),
        };

        Graphics::Shapes::Cylinder cyl(1, 1, 20, 
                MeshMode::Pos3);
        auto verts = cyl.getFullVertices();
        auto ind = cyl.getIndices();


        Graphics::VertexBufferLayout layout; 
        layout.push(Graphics::Format::FLOAT3, 0);

        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);

        Shader defaultShader;

        defaultShader.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");


        defaultShader.addUniform(color);

        defaultShader.addPushConstant(mvp);

        mRenderer->addShader(defaultShader); 

        std::vector<float> planeVerts = {
            -0.5, 0, 0.5,
            0.5, 0, 0.5,
            -0.5, 0, -0.5,
            0.5, 0, -0.5,
        };

        std::vector<u32> planeInd = {2, 1, 0, 2, 3, 1};

        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void DynamicCollisionLayer::run()
    {
        mGameWorld.update();
    }


    void DynamicCollisionLayer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void DynamicCollisionLayer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void DynamicCollisionLayer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}


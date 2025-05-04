#include "ai_layer.h"
namespace Lina{ namespace Graphics{
    void AiLayer::init()
    {
        // Renderer
        Uniform color = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Dynamic,
            .size = sizeof(f32) * 4,
            .count = 5,
        };

        PushConstant time = 
        {
            .stage = ShaderStage::Fragment,
            .offset = 0,
            .size = 3 * sizeof(float) 
        };


        PushConstant mvp = 
        {
            .stage = ShaderStage::Vertex, 
            .offset = 0,
            .size = sizeof(Math::Transform4D),
        };

        Graphics::Shapes::Cylinder cyl(1, 1, 20, 
                MeshMode::Pos3, {.col = {1.0f, 0.2f, 0.3f}});

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


        Core::TinyBasedObjLoader objLoader;
        objLoader.load("../assets/env.obj");

        auto verts2 = objLoader.getPositions();
        auto ind2 = objLoader.getIndices();

        objLoader.setNavPath("../assets/env_nav.obj");
        auto envnormals = objLoader.getNormals();
        mGeoModifier.setVertices(verts2);
        //mGeoModifier.setNormals(envnormals);
       // mGeoModifier.setNormalMode(ModifierNormals::Provided);
        mGeoModifier.init();
        mGeoModifier.setMulFactor(0.5f);
        mGeoModifier.warp(0.25, 2.5f, 10, 0.2);
        verts2 = mGeoModifier.getVertices();

        auto graph = objLoader.getGraph();

        mRenderer->createVertexBuffer(layout, verts2);
        mRenderer->createIndexBuffer(ind2);

        std::vector<f32> screenVerts = {
            -1, 1,
            1, 1,
            -1, -1,
            1, -1,
        };
        
        Graphics::VertexBufferLayout screenLayout; 
        screenLayout.push(Graphics::Format::FLOAT2, 0);

        std::vector<u32> screenInds = {0, 1, 2, 1, 3, 2};
        mRenderer->createVertexBuffer(screenLayout, screenVerts);
        mRenderer->createIndexBuffer(screenInds);

        Shader effectShader;

        effectShader.init("../shaders/compiled/effect.vert.spv",
                "../shaders/compiled/effect.frag.spv", "shader");
        //effectShader.addUniform();
        effectShader.addPushConstant(time);
        mRenderer->addShader(effectShader); 

        mRenderer->addMap(0, 0);
        mRenderer->addMap(0, 1);
        mRenderer->addMap(1, 2);
        
        // Game World
        mGameWorld.setGraphs({graph});
        mGameWorld.setEnvVerts(verts2);
        mGameWorld.setEnvInds(ind2);
        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void AiLayer::run()
    {
        mGameWorld.update();
    }


    void AiLayer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void AiLayer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void AiLayer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}

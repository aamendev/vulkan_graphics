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

        /*std::vector<float> verts2 = {
          -0.5, 0, 0.5,
          0.5, 0, 0.5,
          -0.5, 0, -0.5,
          0.5, 0, -0.5,
          };

          std::vector<u32> ind2 = {2, 1, 0, 2, 3, 1};*/

        Core::TinyBasedObjLoader objLoader;
        objLoader.load("../assets/env_nav.obj");
        objLoader.setNavPath("../assets/env_nav.obj");

        auto verts2 = objLoader.getPositions();
        auto ind2 = objLoader.getIndices();
        auto graph = objLoader.getGraph();

        mRenderer->createVertexBuffer(layout, verts2);
        mRenderer->createIndexBuffer(ind2);

        Shader defaultShader2;

        defaultShader2.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");

        defaultShader2.addUniform(color2);

        defaultShader2.addPushConstant(mvp);

        mRenderer->addShader(defaultShader2); 
        
        // Game World
        mGameWorld.setGraphs({graph});
        mGameWorld.setEnvVerts(verts2);
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

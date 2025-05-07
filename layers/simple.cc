#include "simple.h"
namespace Lina{ namespace Graphics{
    void SimpleLayer::init()
    {
        // Renderer
        PushConstant mvp = 
        {
            .stage = ShaderStage::Vertex, 
            .offset = 0,
            .size = sizeof(Math::Transform4D),
        };


        PushConstant col 
        {
            .stage = ShaderStage::Fragment,
            .offset = 64,
            .size = sizeof(f32) * 4,
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

        defaultShader.init("../shaders/compiled/simplest.vert.spv",
                "../shaders/compiled/simplest.frag.spv", "shader");
        defaultShader.addPushConstant(mvp);
        defaultShader.addPushConstant(col);
        mRenderer->addShader(defaultShader); 

        std::vector<f32> triVerts = {
            -0.5f, 0.5f, 0.1f,
            0.0f, -0.5f, 0.1f,
            0.5f, 0.5f, 0.1f
        };
        
        std::vector<u32> triInds = {0, 1, 2};

        mGeoModifier.setVertices(triVerts);
        std::vector<Math::Vector3D> norms = {Math::Vector3D(0,0,1),
        Math::Vector3D(0,0,1),
        Math::Vector3D(0,0,1)};
        mGeoModifier.setNormals(norms);
        mGeoModifier.setNormalMode(ModifierNormals::Provided);
        mGeoModifier.init();
        mGeoModifier.setThreshold(1.0f);
        mGeoModifier.setMulFactor(2.3f);
        mGeoModifier.fbm(1.8f, 1.2f, 8);
        mGeoModifier.combineVertices();
        triVerts = mGeoModifier.getVertices();

        mRenderer->createVertexBuffer(layout, triVerts);
        mRenderer->createIndexBuffer(triInds);

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

        Shader postprocessShader;

        postprocessShader.init("../shaders/compiled/simplestpostprocess.vert.spv",
                "../shaders/compiled/simplestpostprocess.frag.spv", "shader");
        postprocessShader.loadMainTexture();
        mRenderer->addShader(postprocessShader); 

        mRenderer->addMap(0, 0);
        mRenderer->addMap(0, 1);
        mRenderer->addMap(1, 2);
        
        // Game World
        mGameWorld.setEnvVerts(triVerts);
        mGameWorld.setEnvInds(triInds);
        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void SimpleLayer::run()
    {
        mGameWorld.update();
    }


    void SimpleLayer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void SimpleLayer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void SimpleLayer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}

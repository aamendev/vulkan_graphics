#include "final_layer.h"
namespace Lina{ namespace Graphics{
    void FinalLayer::init()
    {
        // Renderer
        PushConstant rayTracer = 
        {
            .stage = ShaderStage::Fragment,
            .offset = 0, 
            .size = sizeof(f32) * (4*4 + 2)
        };
        Uniform color = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Dynamic,
            .size = sizeof(f32) * 4,
            .count = 1,
        };
        Uniform planetColours = 
        {
            .name = "planetcols",
            .binding = 1,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Dynamic,
            .size = sizeof(f32) * 4 * 3,
            .count = 3,
        };
        TextureData noiseTexture = 
        {
            .name = "noise",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .source = TextureSource::Noise,
        };

        PushConstant separateMvp = 
        {
            .stage = ShaderStage::Vertex,
            .offset = 0,
            .size = sizeof(Math::Transform4D)*2
        };
        Uniform planetLights =
        {
            .name = "planetlights",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Static,
            .size = sizeof(Math::Vector4D) * 3,
            .count = 1
        };

        PushConstant time = 
        {
            .stage = ShaderStage::Fragment,
            .offset = 0,
            .size = 3 * sizeof(float) 
        };


        PushConstant offsetTime = 
        {
            .stage = ShaderStage::Fragment,
            .offset = sizeof(Math::Transform4D),
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


        Graphics::Shapes::Icosphere ico(0.4f);
        ico.setMeshMode(MeshMode::Pos3);
        ico.subdivide(5);

        auto verts2 = ico.getFullVertices();
        auto colverts = ico.getFullVertices();
        auto ind2 = ico.getIndices();
        auto planetnorms = ico.getNormals();

        VertexBufferLayout layout2;
        layout2.push(Graphics::Format::FLOAT3, 0);
        layout2.push(Graphics::Format::FLOAT3, 1);
        mGeoModifier.setVertices(verts2);
        
        mGeoModifier.setNormals(planetnorms);
        mGeoModifier.setNormalMode(ModifierNormals::Provided);
        mGeoModifier.init();
        mGeoModifier.setThreshold(0.75f);
        mGeoModifier.setMulFactor(0.1f);
        mGeoModifier.warp(0.8f, 1.2f, 12, 0.7f);
       // mGeoModifier.fbm(1.8f, 1.2f, 12);
        mGeoModifier.combineVertices();
        verts2 = mGeoModifier.getVertices();
        auto planetVerts = mGeoModifier.getFullVertices();

        

        mRenderer->createVertexBuffer(layout2, planetVerts);
        mRenderer->createIndexBuffer(ind2);

        Shader planetShader;

        planetShader.init("../shaders/compiled/planet.vert.spv",
                "../shaders/compiled/planet.frag.spv", "shader");
        planetShader.addUniform(planetLights);
        planetShader.addUniform(planetColours);
        planetShader.addPushConstant(separateMvp);
        mRenderer->addShader(planetShader); 

        VertexBufferLayout gasLayout;
        gasLayout.push(Graphics::Format::FLOAT4, 0);
        mColModifier.setVertices(colverts);
        mColModifier.init();
        mColModifier.setThreshold(1.0f);
        mColModifier.setMulFactor(1.0f);
        mColModifier.fbm(0.8f, 1.2f, 8);
        mColModifier.combineVertices();
        colverts = mColModifier.getFullVertices();
        mRenderer->createVertexBuffer(gasLayout, colverts);
        mRenderer->createIndexBuffer(ind2);
        Shader fogShader;

        fogShader.init("../shaders/compiled/effect3.vert.spv",
                "../shaders/compiled/effect3.frag.spv", "shader");
        fogShader.addPushConstant(mvp);
        mRenderer->addShader(fogShader); 

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

        postprocessShader.init("../shaders/compiled/postprocess.vert.spv",
                "../shaders/compiled/postprocess.frag.spv", "shader");
        postprocessShader.addPushConstant(rayTracer);
        postprocessShader.addTexture(noiseTexture);
        postprocessShader.loadMainTexture();
        mRenderer->addShader(postprocessShader); 


        // simple, cyl
        // planet, sphere
        // simple, sphere 
        // fog, sphere
        mRenderer->addMap(0, 0);
        mRenderer->addMap(1, 1);
        mRenderer->addMap(0, 1);// 3
        mRenderer->addMap(2, 2);
        mRenderer->addMap(3, 3); 
        
        // Game World
        mGameWorld.setEnvVerts(verts2);
        mGameWorld.setEnvInds(ind2);
        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void FinalLayer::run()
    {
        mGameWorld.update();
    }


    void FinalLayer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void FinalLayer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void FinalLayer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}

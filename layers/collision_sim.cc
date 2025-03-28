#include "collision_sim.h"
#include "../core/tiny_based_obj_loader.h"
namespace Lina{ namespace Graphics{
    void CollisionSimLayer::init()
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

        Graphics::Shapes::Icosphere ico(1.0f);
        ico.setMeshMode(MeshMode::Pos3);
        ico.subdivide(5);
        auto icoverts = ico.getFullVertices();
        auto icoind = ico.getIndices();

        mRenderer->createVertexBuffer(layout, icoverts);
        mRenderer->createIndexBuffer(icoind);

        Shader psShader;

        psShader.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");


        psShader.addUniform(color);

        psShader.addPushConstant(mvp);

        mRenderer->addShader(psShader);
/*
        psShader.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");


        psShader.addUniform(color);

        psShader.addPushConstant(mvp);

        mRenderer->addShader(psShader);
        */


        Core::TinyBasedObjLoader obj;

        obj.load("../assets/ramp.obj");
        auto envpos = obj.getPositions();
        auto envverts = obj.getFullVertices();
        auto envind = obj.getIndices();
        Graphics::VertexBufferLayout layout2;
        layout2.push(Graphics::Format::FLOAT3, 0);
        layout2.push(Graphics::Format::FLOAT2, 1);
        layout2.push(Graphics::Format::FLOAT3, 2);
        layout2.push(Graphics::Format::FLOAT3, 3);


        mRenderer->createVertexBuffer(layout2, envverts);
        mRenderer->createIndexBuffer(envind);
        Shader defaultShader2;
        defaultShader2.init("../shaders/compiled/terrain.vert.spv",
        "../shaders/compiled/terrain.frag.spv", "shader");

        defaultShader2.addPushConstant(mvp);
        defaultShader2.addUniform(color);
        mRenderer->addShader(defaultShader2); 
       /* 
        obj.load("../assets/env.obj");
        auto envpos2 = obj.getPositions();
        auto envverts2 = obj.getFullVertices();
        auto envind2 = obj.getIndices();


        mRenderer->createVertexBuffer(layout2, envverts2);
        mRenderer->createIndexBuffer(envind2);
        Shader defaultShader3;
        defaultShader3.init("../shaders/compiled/terrain.vert.spv",
        "../shaders/compiled/terrain.frag.spv", "shader");

        defaultShader3.addPushConstant(mvp);
        defaultShader3.addUniform(color);
        mRenderer->addShader(defaultShader3); 
       */ 
        // Game World
        
//        mGameWorld.setEnvVerts({icoverts, envpos, envpos2});
        mGameWorld.setEnvVerts({icoverts, envpos});
        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
    }

    void CollisionSimLayer::run()
    {
        mGameWorld.update();
    }


    void CollisionSimLayer::onKeyDown(Events::KeyPress& e)
    {
        mGameWorld.onKeyDown(e);
    }

    void CollisionSimLayer::onKeyUp(Events::KeyRelease& e)
    {
        mGameWorld.onKeyUp(e);
    }

    void CollisionSimLayer::onMouseMove(Events::MouseMove& e)
    {
        mGameWorld.onMouseMove(e);
    }
}}

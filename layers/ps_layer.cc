#include "ps_layer.h"
#include "../core/obj_loader.h"
namespace Lina { namespace Graphics {

    void PSLayer::init() 
	{
        Core::ObjLoader obj;

   //     obj.load("../assets/teapot.obj");
        obj.load("../assets/viking.obj");
        auto verts = obj.getFullVertices();
        auto ind = obj.getIndices();

        Graphics::Shapes::Icosphere s(1.1f);
/*
        s.setMeshMode(MeshMode::Pos3Col3);
        s.subdivide(5);
        auto verts = s.getFullVertices();
        auto ind = s.getIndices();
*/


        Graphics::VertexBufferLayout layout; 
        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT2, 1);
        layout.push(Graphics::Format::FLOAT3, 2);

        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);

        Shader defaultShader;

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

        defaultShader.init("../shaders/compiled/simple.vert.spv",
                "../shaders/compiled/simple.frag.spv", "shader");
        defaultShader.addPushConstant(mvp);
        defaultShader.addUniform(color);
        mRenderer->addShader(defaultShader); 

        mGameWorld.setRenderer(mRenderer);
        mGameWorld.preinit();
        mGameWorld.init();
	}

    void PSLayer::run() 
	{
        mGameWorld.update();
	}

    void PSLayer::onMouseMove(Events::MouseMove& m) 
	{
        mGameWorld.onMouseMove(m);
	}
    void PSLayer::onKeyDown(Events::KeyPress& k) 
	{
        mGameWorld.onKeyDown(k);
	}
    void PSLayer::onKeyUp(Events::KeyRelease& k) 
	{
        mGameWorld.onKeyUp(k);
	}
}}

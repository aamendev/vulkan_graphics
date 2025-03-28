#include "space.h"
namespace Lina { namespace Graphics {

    void Space::init() 
	{

        Graphics::Shapes::Icosphere s(1.0f);

        s.setMeshMode(MeshMode::Pos3);
        s.subdivide(5);
        auto verts = s.getFullVertices();
        auto ind = s.getIndices();



        Graphics::VertexBufferLayout layout; 
        layout.push(Graphics::Format::FLOAT3, 0);

        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);

        Shader defaultShader;

        Uniform color = 
        {
            .name = "col",
            .binding = 0,
            .stage = ShaderStage::Fragment,
            .type = UniformType::Dynamic,
            .size = sizeof(f32) * 4,
            .count = 3,
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

    void Space::run() 
	{
        mGameWorld.update();
	}

    void Space::onMouseMove(Events::MouseMove& m) 
	{
        mGameWorld.onMouseMove(m);
	}
    void Space::onKeyDown(Events::KeyPress& k) 
	{
        mGameWorld.onKeyDown(k);
	}
    void Space::onKeyUp(Events::KeyRelease& k) 
	{
        mGameWorld.onKeyUp(k);
	}
}}

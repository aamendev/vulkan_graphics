#include "./scene_layer.h"

namespace Lina{ namespace Graphics{
    void SceneLayer::onKeyDown(Events::KeyPress& e)
    {
        auto k = e.key();
        switch(k)
        {
            case Input::KeyCode::O:
                shuttleIndex = 0;
                break;
            case Input::KeyCode::P:
                shuttleIndex = 1;
                break;
            default:{}
        }

        if (shuttleIndex == 0)
        {
        switch (k) {
            case Input::KeyCode::K:
                followIndex = (followIndex + 1) % (9);
                break;
            case Input::KeyCode::L:
                if (followIndex < 1) {followIndex = 9;}
                followIndex--;
                break;
                default:{}
        }
        }
        else
        {
            shuttles[shuttleIndex].update(e);
        }
    }
    void SceneLayer::init()
    {
        fin.open("../assets/planets.csv", std::ios::in);
        fin >> temp; //reomve header
        float damp = 0.06f;
        while (fin >> temp)
        {
            data.clear();
            std::stringstream s(temp);
            while (getline(s, word, ','))
            {
                data.push_back(word);
            }

            pos.emplace_back(stof(data[1]), stof(data[2]), stof(data[3]));
            radii.emplace_back(stof(data[4]));
            orbital_speeds.emplace_back(damp * stof(data[5]));
            selforbital_speeds.emplace_back(damp * 2 * stof(data[6]));
            texData.emplace_back(std::string("../assets/") + data[7], true);
            currTransforms.emplace_back(1.0f, 0, 0, stof(data[1]),
                    0, 1, 0.0f, stof(data[2]),
                    0, 0, 1.0f, stof(data[3]));
        }
        fin.close();

        mRenderer->createTexture(texData);

        Graphics::Shapes::Icosphere ico(3.0f);
        ico.subdivide(5);

        auto verts = ico.getFullVertices();
        auto ind = ico.getIndices();

        std::vector<float> verts2 = {
            -1, 1, 0, 0, 1,
            1, 1, 1, 0, 0,
            -1, -1, 0, 1, 0,
            1, -1, 1, 0, 0,
        };

        std::vector<u32> ind2 = {0, 1, 2, 1, 3, 2};

        Graphics::VertexBufferLayout layout; 
        Graphics::VertexBufferLayout layout2;

        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT3, 1);
        layout.push(Graphics::Format::FLOAT2, 2);

        layout2.push(Graphics::Format::FLOAT2, 0);
        layout2.push(Graphics::Format::FLOAT3, 1);
          
        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);
        mRenderer->createVertexBuffer(layout2, verts2);
        mRenderer->createIndexBuffer(ind2);


        mRenderer->createUniformBuffers(sizeof(fulltransMat));
        mRenderer->createUniformBuffers(sizeof(EffectUniform));
        effectUniform.width = mWindow->getWidth();
        effectUniform.height = mWindow->getHeight();
        effectUniform.time = 0;

        mRenderer->addShader("../shaders/compiled/shader.vert.spv",
                "../shaders/compiled/shader.frag.spv"); 
        mRenderer->addShader("../shaders/compiled/effect.vert.spv",
                "../shaders/compiled/effect.frag.spv"); 

        mRenderer->createGraphicsPipelines();




        Shuttle shuttle;
        Shuttle movingShuttle;

        shuttles = {shuttle, movingShuttle};
        shuttles[1].setPosition(0.0, 0.0, -50.0);
        float theta = 0.0f;
        float speed = 4.0f;
        int i = 0;
        
    }
    void SceneLayer::run()
    {
        auto proj = Math::Util::projMatrix(
                135, 
                mWindow->getWidth() / mWindow-> getHeight(), 
                true);

        mRenderer->beginDraw();
        mRenderer->bindShader(0);
        mRenderer->setPrimitive(Primitive::Triangle);
        for (int i = 0; i < 9; i++)
        {
        
            fulltransMat = 
                proj *
                shuttles[shuttleIndex].getMatrix()*
                currTransforms[i] *
                Math::Util::scaleMatrix({radii[i], radii[i], radii[i]});
            mRenderer->updateUniform(&fulltransMat, 0);
            mRenderer->render(nullptr, nullptr, i);


            // script
            auto curr = currTransforms[i].getTranslation();
            auto c = currTransforms[0].getTranslation();
            Math::Transform4D rotMat =
                Math::Util::rotationMatrix4D(selforbital_speeds[i], Math::Util::yAxis())
                * Math::Util::translationMatrix(c - curr)
                * Math::Util::rotationMatrix4D(orbital_speeds[i], Math::Util::yAxis())
                * Math::Util::translationMatrix(curr - c);
            currTransforms[i] = rotMat * currTransforms[i];
            // end script
        }
        //script
        auto followTransform = currTransforms[followIndex].getTranslation();
        shuttles[0].setPosition(followTransform.x, followTransform.y, followTransform.z - 15*radii[followIndex]);
        // end script

        mRenderer->bindShader(1);
        effectUniform.width = mWindow->getWidth();
        effectUniform.height = mWindow->getHeight();
        effectUniform.time += 0.001f;
        mRenderer->updateUniform(&effectUniform, 1);

        mRenderer->render();

        mRenderer->endDraw();
    }
}}

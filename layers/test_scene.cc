#include "./test_scene.h"

namespace Lina{ namespace Graphics{
    void TestSceneLayer::onKeyDown(Events::KeyPress& e)
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
    void TestSceneLayer::init()
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

        Graphics::VertexBufferLayout layout; 

        layout.push(Graphics::Format::FLOAT3, 0);
        layout.push(Graphics::Format::FLOAT3, 1);
        layout.push(Graphics::Format::FLOAT2, 2);


        mRenderer->createVertexBuffer(layout, verts);
        mRenderer->createIndexBuffer(ind);
        mRenderer->createGraphicsPipeline();
        mRenderer->createUniformBuffers(sizeof(fulltransMat));

        Shuttle shuttle;
        Shuttle movingShuttle;

        shuttles = {shuttle, movingShuttle};
        shuttles[1].setPosition(0.0, 0.0, -50.0);
        float theta = 0.0f;
        float speed = 4.0f;
        int i = 0;
    }
    void TestSceneLayer::run()
    {
        std::vector<f32> planeVerts = 
        {0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 1.0, 0.0, 0.0,
            1.0, 1.0, 0.0, 1.0, 0.0, 0.0,
            1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
        std::vector<u32> planInds = {
            0, 1, 2, 
            2, 3, 0}; 

        Graphics::VertexBufferLayout planLayout;
        planLayout.push(Graphics::Format::FLOAT3, 0);
        planLayout.push(Graphics::Format::FLOAT3, 1);

        VertexBuffer* planevertBuffer = new VertexBuffer();
        IndexBuffer* planeindBuffer = new IndexBuffer();
        mRenderer->createVertexBuffer(planLayout, planeVerts, planevertBuffer);
        mRenderer->createIndexBuffer(planInds, planeindBuffer);

        auto proj = Math::Util::projMatrix(135, mWindow->getWidth() / mWindow-> getHeight(), true);
        mRenderer->beginDraw();
        mRenderer->setPrimitive(Primitive::Triangle);

        mRenderer->render(nullptr, nullptr, 2);
        for (int i = 0; i < 9; i++)
        {
            auto followTransform = currTransforms[followIndex].getTranslation();
            shuttles[0].setPosition(followTransform.x, followTransform.y, followTransform.z - 15*radii[followIndex]);
            fulltransMat = 
                proj *
                shuttles[shuttleIndex].getMatrix()*
                currTransforms[i] *
                Math::Util::scaleMatrix({radii[i], radii[i], radii[i]});
            mRenderer->updateUniform(&fulltransMat);
            //            mRenderer->render(nullptr, nullptr, i);
            mRenderer->render(planevertBuffer, planeindBuffer, i);
            auto curr = currTransforms[i].getTranslation();
            auto c = currTransforms[0].getTranslation();
            Math::Transform4D rotMat =
                Math::Util::rotationMatrix4D(selforbital_speeds[i], Math::Util::yAxis())
                * Math::Util::translationMatrix(c - curr)
                * Math::Util::rotationMatrix4D(orbital_speeds[i], Math::Util::yAxis())
                * Math::Util::translationMatrix(curr - c);

            currTransforms[i] = rotMat * currTransforms[i];
        }
            //mRenderer->updateUniform(&currTransforms[0]);
            //mRenderer->render(planevertBuffer, planeindBuffer, 0);
        mRenderer->endDraw();
    }
}}

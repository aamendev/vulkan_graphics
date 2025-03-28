#include "ps.h"

namespace Lina { namespace Games {

    void PSWorld::preinit() 
    {
        mParticleSystem.setMaxLifeTime(10.0f);
        mParticleSystem.setRadius(20);
        mParticleSystem.setParticleCount(800);
        mParticleSystem.setMinRotation({0, 0, 0});
        mParticleSystem.setMaxRotation({0, 0, PI/8});
        mParticleSystem.setMinV({1.0f, 2.0f, 0.0f});
        mParticleSystem.setMaxV({1.5f, 10.0f, 0.0f});


        Physics::Particle p;
        Physics::Particle p2;

        mParticles.push_back(p);
       // mParticles.push_back(p2);

        auto restLength = 12.0f;
        mParticles[0].setPosition({0, 0, 0});
        /*
        mSpring.setAnchor(mParticles[1].getPosPointer());
        mSpring2.setAnchor(mParticles[0].getPosPointer());
        mSpring.setRestLength(restLength);
        mSpring2.setRestLength(restLength);

        mParticles[0].addVariableForce(&mSpring);
        mParticles[1].addVariableForce(&mSpring2);
        mParticles[0].setPosition({0, 20, 0});
        mParticles[1].setDamping(0.99f);
        mParticles[0].setDamping(0.99f);
        
        mSpring.attachPoint(mParticles[0].getPosPointer());
        mSpring2.attachPoint(mParticles[1].getPosPointer());
        */
/*
        auto gravCount = 2;
        mGravity.setG(100.0f);
        mGravity.setMinDistance(0.001f);
        mParticles[0].setMass(2);
       // mParticles[1].setMass(20);
        for (int i = 0; i < gravCount; i++)
        {
            mGravity.addBody(&mParticles[i]);
        }

        mParticles[0].addVariableForce(&mGravity);
        mParticles[0].setVelocity({10.0f, 0, 0});
        */

        //mParticles[0].applyForce({1, 0, 0});
        //mParticles[1].setVelocity({4, 0, 0});
       mShuttle.setPosition(0, 0, -2);
    }
    void PSWorld::init() 
    {
        mParticleSystem.init();
    }

    void PSWorld::onKeyDown(Events::KeyPress& e)
    {
        mShuttle.onKeyDown(e);
        switch(e.key())
        {
            case Input::KeyCode::W:
                //mZoom -= 0.11f;
                break;
            case Input::KeyCode::S:
               // mZoom += 0.11f;
                break;
            default:{}
        }
    }

    void PSWorld::onMouseMove(Events::MouseMove& e)
    {
        std::cerr << "Mouse Move: (" << e.x() << ", " << e.y() << ")\n"; 
        mShuttle.onMouseMove(e);
    }
    void PSWorld::update()
    {
        typedef struct color 
        {
            f32 r = 0.0f;
            f32 g = 1.0f;
            f32 b = 1.0f;
            f32 a = 1.0f;
        } Col;

        Col col;

       auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);
        auto scale = Math::Util::scaleMatrix({0.1f, 1.0f, 0.1f});
        auto scale2 = Math::Util::scaleMatrix({1, 1, 1});

        mTimer.begin();

       for (auto& p : mParticles)
       {
           p.update(1.0f / mFrameRate);
       }
        mRenderer->beginDraw();
        mRenderer->updateUniform(&col, 0);
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->bindShader(0);

        for (auto& p : mParticles)
        {
        auto trans = proj * mShuttle.getMatrix() * 
            Math::Util::transMatrix(Math::Util::identityMatrix(),
                    p.getPos())  * Math::Util::rotationMatrix4D(
                    0 * PI / 180, {0, 0, 1}) * 
                        Math::Util::rotationMatrix4D(0 * PI / 180, {0, 1, 0}) *  scale2;
        mRenderer->updatePushConstant(&trans, 0);
        mRenderer->render();
        }

        auto followidx = 0;

        auto followPos = mParticles[followidx].getPos();
        auto shuttlePos = mShuttle.getPos();
        //mShuttle.setPosition(followPos.x, followPos.y, shuttlePos.z);
        mRenderer->endDraw();

        mTimer.end();
        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }
}}

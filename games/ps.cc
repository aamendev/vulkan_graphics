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


        //mSpring.setAnchor(mParticle2.getPosPointer());
       // mSpring2.setAnchor(mParticle.getPosPointer());
        //mParticle.addVariableForce(&mSpring);
       // mParticle2.addVariableForce(&mSpring2);
        //mParticle.setPosition({0, 20, 0});
      //  mParticle.applyForce({1, 0, 0});
        //mParticle2.setVelocity({1, 0, 0});
       // mSpring.attachPoint(mParticle.getPosPointer());
       // mSpring2.attachPoint(mParticle2.getPosPointer());
       mShuttle.setPosition(0, -5, -20);
    }
    void PSWorld::init() 
    {
        mParticleSystem.init();
    }

    void PSWorld::onKeyDown(Events::KeyPress& e)
    {
        mShuttle.update(e);
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
       // mParticleSystem.update(1.0f / 60);
        mParticle2.update(1 / mFrameRate);
        mParticle.update(1.0f / mFrameRate);
        //auto& ps = mParticleSystem.getParticles();
       // auto& rots = mParticleSystem.getRotations();
        mRenderer->beginDraw();
        mRenderer->updateUniform(&col, 0);
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->bindShader(0);
        /*
        for (int i = 0; i < ps.size(); i++)
        {
           auto trans = proj * mShuttle.getMatrix() *
               Math::Util::transMatrix(Math::Util::identityMatrix(), 
                   ps[i].getPos()) 
               * Math::Util::transMatrix((Math::Quatrenion::angleToQuat(rots[i])).getRotationMatrix(),
                        {0, 0, 0}) * scale; 
            mRenderer->updatePushConstant(&trans, 0, 0);
            mRenderer->render();
        }
        */

        auto trans = proj * mShuttle.getMatrix() * 
            Math::Util::transMatrix(Math::Util::identityMatrix(),
                    mParticle.getPos())  * Math::Util::rotationMatrix4D(
                    0 * PI / 180, {0, 0, 1}) * 
                        Math::Util::rotationMatrix4D(0 * PI / 180, {0, 1, 0}) *  scale2;
        mRenderer->updatePushConstant(&trans, 0);
        mRenderer->render();


        auto followPos = mParticleSystem.getPosition();
       // mShuttle.setPosition(0, 100, 
         //       - mZoom * 4);
        mRenderer->endDraw();

        mTimer.end();
        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }
}}

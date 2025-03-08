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
    }
    void PSWorld::init() 
    {
        mParticleSystem.init();
    }

    void PSWorld::onKeyDown(Events::KeyPress& e)
    {
        switch(e.key())
        {
            case Input::KeyCode::W:
                mZoom -= 0.11f;
                break;
            case Input::KeyCode::S:
                mZoom += 0.11f;
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
        mTimer.begin();
        mParticleSystem.update(1.0f / 60);
        auto& ps = mParticleSystem.getParticles();
        auto& rots = mParticleSystem.getRotations();
        mRenderer->beginDraw();
        mRenderer->updateUniform(&col, 0, 0);
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->bindShader(0);
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
        auto followPos = mParticleSystem.getPosition();
        mShuttle.setPosition(followPos.x, followPos.y + 20.0f, 
                followPos.z - mZoom * mParticleSystem.getRadius());
        mRenderer->endDraw();
        mTimer.end();
        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }
}}

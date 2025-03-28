#include "space.h"

namespace Lina { namespace Games {

    void Space::preinit() 
    {
        mShuttle.setPosition(0, 0, -2);
        addPlanet(1.989e10 * 4, false, {0, 0, 0}, 1.0);
        addPlanet(5.972e4 * 4, true, {0, 15.0 / 2, 0}, 0.3);
        addPlanet(7.34e2 * 4, true, {0, 15.0/ 2 + 3e-3, 0}, 0.01);
        fetchGravity();
        mParticles[1].setVelocity({22.5e-2 * 2, 0, 0});
        mParticles[2].setVelocity({22.e-2 * 2, 0, 0});

    }

    void Space::fetchGravity()
    {
        mGravity.reserve(mGravityCount);
        for (u32 i = 0; i < mParticles.size(); i++)
        {
            if (mHasGravity[i])
            {
                mGravity.emplace_back();
                auto last = mGravity.size() - 1;
                mGravity[last].setG(2e-11);
                mGravity[last].setMinDistance(0.0);
                mGravity[last].setMainBody(i);
                for (u32 j = 0; j < mParticles.size(); j++)
                {
                    mGravity[last].addBody(&mParticles[j]);
                }
                mParticles[i].addVariableForce(&mGravity[last]);
            }
        }
    }

    void Space::addPlanet(f32 mass, b8 addOwnGravity, Math::Point3D&& pos, f32 r)
    {
        mScales.push_back(r);
        Physics::Particle p;
        p.setPosition(std::move(pos));
        p.setMass(mass);
        mParticles.push_back(p);
            
            mHasGravity.push_back(addOwnGravity);
        mColours.push_back(
                {
                .r = Core::Utils::randFloat(0, 1),
                .g = Core::Utils::randFloat(0, 1),
                .b = Core::Utils::randFloat(0, 1),
                .a = 1,
                }
                );
        mGravityCount += addOwnGravity;
    }

    void Space::init() 
    {
    }

    void Space::onKeyDown(Events::KeyPress& e)
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

    void Space::onMouseMove(Events::MouseMove& e)
    {
        mShuttle.onMouseMove(e);
    }
    void Space::update()
    {

        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);
        auto scale = Math::Util::scaleMatrix({0.1f, 1.0f, 0.1f});
        auto scale2 = Math::Util::scaleMatrix({1, 1, 1});

        mTimer.begin();

        for (auto& p : mParticles)
        {
            for (int i = 0; i < 10; i++)
            p.update(1.0f / mFrameRate);
        }
        mRenderer->beginDraw();
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->bindShader(0);

        for (int i = 0; i < mParticles.size() ; i++)
        {
            mRenderer->updateUniform(&mColours[i], 0, i);
            auto trans = proj * mShuttle.getMatrix() * 
                Math::Util::transMatrix(Math::Util::identityMatrix(),
                        mParticles[i].getPos())  * Math::Util::rotationMatrix4D(
                        0 * PI / 180, {0, 0, 1}) * 
                            Math::Util::rotationMatrix4D(0 * PI / 180, {0, 1, 0}) *  
                            Math::Util::scaleMatrix({mScales[i], mScales[i], mScales[i]});
            mRenderer->updatePushConstant(&trans, 0);
            mRenderer->render();
        }

        auto followidx = 1;

        auto followPos = mParticles[followidx].getPos();
        auto shuttlePos = mShuttle.getPos();
        mShuttle.setPosition(followPos.x, followPos.y, shuttlePos.z);
        mRenderer->endDraw();

        mTimer.end();
        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }
}}

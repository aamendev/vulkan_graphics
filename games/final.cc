#include "final.h"
#include <cmath>
#include <string>

namespace Lina { namespace Games {

    void FinalGameWorld::preinit()
    {
        Physics::Particle* earthParticle = new Physics::Particle();
        earthParticle->setPosition({400, 0, 0});
        earthParticle->setMass(10.0f);

        f32 sphereTens = (2.0f/5) * earthParticle->getMass() *  200*200;
        Math::Matrix3D inertiaTensor(
                sphereTens, 0, 0,
                0, sphereTens, 0,
                0, 0, sphereTens);
        Physics::RbSetting earthRbSettings = 
        {
            .particle = earthParticle,
            .invInertiaTensor = inertiaTensor.inverse()
        };
        
        Physics::RigidBody* earthRb = new Physics::RigidBody(earthRbSettings);
        Planet earth = 
        {
            .tag = "earth",
            .rb = earthRb,
            .rad = 50.0f,
            .colLayers = {OCEAN, GREEN, DIRT},
            .isStar = false,
        };

        Physics::Particle* sunParticle = new Physics::Particle();
        sunParticle->setPosition({0, 0, 0});
        sunParticle->setMass(200.0f);

        sphereTens = (2.0f/5) * sunParticle->getMass() *  200*200;
        inertiaTensor = 
        {
                sphereTens, 0, 0,
                0, sphereTens, 0,
                0, 0, sphereTens
        };
        Physics::RbSetting sunRbSettings = 
        {
            .particle = sunParticle,
            .invInertiaTensor = inertiaTensor.inverse()
        };
        
        Physics::RigidBody* sunRb = new Physics::RigidBody(sunRbSettings);

        Planet sun = 
        {
            .tag = "sun",
            .rb = sunRb,
            .rad = 200.0f,
            .colLayers = {SUN_ORANGE, SUN_ORANGE, SUN_ORANGE},
            .isStar = true,
        };

        Physics::Particle* icyParticle = new Physics::Particle();
        icyParticle->setPosition({600, 0, 0});
        icyParticle->setMass(1.0f);

        sphereTens = (2.0f/5) * icyParticle->getMass() *  200*200;
        inertiaTensor = {
                sphereTens, 0, 0,
                0, sphereTens, 0,
                0, 0, sphereTens};
        Physics::RbSetting iceRbSettings = 
        {
            .particle = icyParticle,
            .invInertiaTensor = inertiaTensor.inverse()
        };
        
        Physics::RigidBody* iceRb = new Physics::RigidBody(iceRbSettings);

        Planet icy = 
        {
            .tag = "icy",
            .rb = iceRb,
            .rad = 25.0f,
            .colLayers = {WHITE, LIGHT_ICE, ICE_TOP}
        };

        mLightData.lightPos.w = sun.rad * 1.2f;

        addPlanet(sun);
        addPlanet(earth);
        addPlanet(icy);

        f32 earthOrb = std::sqrt((mG * sunParticle->getMass()) / 
                (earthParticle->getPos() - sunParticle->getPos()).magnitude());

        earthRb->setLinearVelocity({0, 0, earthOrb});
        earthRb->setAngularVelocity(Math::Util::yAxis() * 0.40f);

        sunRb->setAngularVelocity(Math::Util::yAxis() * 0.05f);
        iceRb->setAngularVelocity(Math::Util::yAxis() * 0.70f);

        Physics::Forces::Gravity* gEarth = new Physics::Forces::Gravity();
        gEarth->setG(mG);
        gEarth->addBody(earthParticle);
        gEarth->addBody(sunParticle);
        gEarth->addBody(icyParticle);
        earthRb->applyForce(gEarth, {0,0,0});

        Physics::Forces::Gravity* gSun = new Physics::Forces::Gravity();
        /*
        gSun->setG(mG);
        gSun->addBody(sunParticle);
        gSun->addBody(earthParticle);
        gSun->addBody(icyParticle);
        sunParticle->addVariableForce(gSun);
        */

        
        Physics::Forces::Gravity* gIce = new Physics::Forces::Gravity();
        gIce->setG(mG);
        gIce->addBody(icyParticle);
        gIce->addBody(sunParticle);
        //gIce->addBody(earthParticle);
        f32 iceOrb = std::sqrt((mG * sunParticle->getMass()) / 
                (icyParticle->getPos() - sunParticle->getPos()).magnitude());

        iceRb->setLinearVelocity({0, 0, iceOrb});
        iceRb->applyForce(gIce, {0,0,0});
        


        /*
        for (auto& [_, value] : mPlaneColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }
        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }
        


        cSystem->staticOptimize();
        f32 rad = 600;
        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();
        */

        mParticleSystem.setMaxLifeTime(10.0f);
        mParticleSystem.setRadius(icy.rad * 1.2f);
        mParticleSystem.setPosition(icy.rb->getPos());
        mParticleSystem.setParticleCount(200);
        mParticleSystem.setMinRotation({0, 0, 0});
        mParticleSystem.setMaxRotation({0, 0, PI/8});
        mParticleSystem.setMinV({1.0f, 1.0f, 0.0f});
        mParticleSystem.setMaxV({1.2f, 2.0f, 0.0f});
    }

    void FinalGameWorld::addPlanet(Planet& p)
    {
        std::string newTag = p.tag;
        std::vector<f32> newVerts(mEnvVerts.size()); 
        std::copy(mEnvVerts.begin(), mEnvVerts.end(), newVerts.begin());
        mPlanets.push_back(p);

        if (p.tag.length() == 0)
        {
        if (!p.isStar)
        {
            newTag = "planet" + std::to_string(mPlanetCount++);
        }
        else 
        {
            newTag = "star" + std::to_string(mStarCount++);
        }
        }
        GameSystem::GameObject planet(newTag);
        planet.addComponent(ComponentType::Transform);
        planet.addComponent(ComponentType::MeshCollider);
        planet.addComponent(ComponentType::Material);
        addObject(planet);
        fetchComponents();
        /*
        mMeshColliderComponents[newTag].setScale({p.rad, p.rad, p.rad});
        mMeshColliderComponents[newTag].setVertices(newVerts);
        mMeshColliderComponents[newTag].setIndices(mEnvInds);
        mMeshColliderComponents[newTag].setPosition(p.pos);
        */
        mTransformComponents[newTag].setScale({p.rad, p.rad, p.rad});
        mTransformComponents[newTag].setPosition(p.rb->getPos());
        mMaterialComponents[newTag].setDynamic(true);

        for (auto& c : p.colLayers)
        {
            mMaterialComponents[newTag].addColour(c);
        }
        mMaterialComponents[newTag].setShader(1);
    }


    void FinalGameWorld::setCallBacks()
    {
        
    }

    void FinalGameWorld::init()
    {
        mShuttles[0].enableRotations();
        mShuttles[1].enableRotations();
        mShuttles[0].enableMovement();
        mShuttles[0].setSpeed(1.0f);
        auto newPos = 
        mPlanets[0].rb->getPos() - Math::Vector3D(0, mPlanets[0].rad - 1.0f, 0);
        mShuttles[0].setPosition(newPos);
        mShuttles[1].setPosition(newPos);
        mShuttles[0].setSenitivity(0.5f);
        mParticleSystem.init();

        translationRate = 0.2f * !physicsMode + 0.2f * physicsMode; 
        mJump = 1.2f * !physicsMode + 200.0f * physicsMode; 
    }

    void FinalGameWorld::reset()
    {
        init();
    }

    void FinalGameWorld::onKeyUp(Events::KeyRelease& e)
    {
        
    }

    void FinalGameWorld::onKeyDown(Events::KeyPress& e)
    {
        mShuttles[mCurrentShuttle].onKeyDown(e);
        switch(e.key())
        {
            case Input::KeyCode::O:
                {
                mFollowIdx++;
                mFollowIdx %= mPlanets.size();
                mZoom = 5.0f;
                mShuttles[1].setRotation({0, 0, 0});
                }
                break;
            case Input::KeyCode::P:
                {
                mFollowIdx--;
                mFollowIdx = (mPlanets.size() - 1) * (mFollowIdx < 0)
                + mFollowIdx * !(mFollowIdx < 0);
                mZoom = 5.0f;
                mShuttles[1].setRotation({0, 0, 0});
                }
                break;
            case Input::KeyCode::B:
                {
                   mCurrentShuttle = !mCurrentShuttle; 
                }
                break;
            case Input::KeyCode::G:
                {
                    mZoom += 1.0f;
                }
                break;
            case Input::KeyCode::H:
                {
                    mZoom -= 1.0f;
                }
            default: {}
        }
    }
    void FinalGameWorld::move()
    {
        mParticleSystem.setPosition(mPlanets[2].rb->getPos());

        for (auto& p : mPlanets)
        {
            p.rb->update(1.0f/mFrameRate);
            mTransformComponents[p.tag].setPosition(p.rb->getPos());
            mTransformComponents[p.tag].setRotation(p.rb->getRot());
        }
        auto currV = mPlanets[2].rb->getLinearVelocity();
        mParticleSystem.setMinV(
                currV 
                + (Math::Vector3D){1.0f, 1.0f, 0.0f});

        mParticleSystem.setMaxV(
                currV 
                +  (Math::Vector3D){1.2f, 2.0f, 0.0f});
    }
    void FinalGameWorld::update()
    {
        mTimer.begin();

        move();
        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);
        auto zoom = mPlanets[mFollowIdx].rad + mZoom;
        auto shuttleFollow = mPlanets[mFollowIdx].rb->getPos();

        mShuttles[1].setPosition(shuttleFollow.x, shuttleFollow.y - 1.0f, shuttleFollow.z - zoom);

        auto& pos = mShuttles[mCurrentShuttle].getPos();
        auto dir = pos + mShuttles[mCurrentShuttle].getTarget();
        auto& up = mShuttles[mCurrentShuttle].getUp();
        auto& earthPos = 
            mTransformComponents["earth"].getPosition();

        mTraceData.rayOrg = Math::Vector4D(pos.x, pos.y, pos.z, 0.0f);
        //mTraceData.rayUp = Math::Vector4D(up.x, up.y, up.z, 0.0f);
        mTraceData.sphereOrg = {earthPos.x, earthPos.y, earthPos.z, 
            mPlanets[1].rad};
        //mTraceData.rayDirection = Math::Vector4D(earthPos.x, earthPos.y, earthPos.z, 0.0f);
        mTraceData.rayDirection = {dir.x, dir.y, dir.z, 0};
        mTraceData.w = mRenderer->getWidth();
        mTraceData.h = mRenderer->getHeight();

        
        

        mParticleSystem.update(1/mFrameRate);
        //Begin Pass System



        mRenderer->beginPasses();
        mRenderer->setPrimitive(Primitive::Triangle);
        int planetCount = 0;
        mRenderer->bindShader(1);
        for (auto& [key, value] : mMaterialComponents)
        {
            if (value.getShaderId() == 1)
            {
                auto currentTransMat = mTransformComponents[key].getMatrix()
                    * Math::Util::scaleMatrix(mTransformComponents[key].getScale()); 
                auto projMatrix = proj * mShuttles[mCurrentShuttle].getMatrix();
                std::array<Math::Matrix4D, 2> ps = {currentTransMat, projMatrix};

                mRenderer->updatePushConstant(ps.data(), 0);
                auto* col = value.getMultipleColoursPointer();
                mRenderer->updateUniform(&mLightData, 0);
                mRenderer->updateUniform((void*)col, 1, planetCount++);
                mRenderer->submitUniformUpdates();
                mRenderer->render();
            }
        }
        // Pass PS:
        auto col = WHITE;
        mRenderer->bindShader(2);
        mRenderer->updateUniform(&col, 0);
        mRenderer->submitUniformUpdates();
        auto& rots = mParticleSystem.getRotations();
        auto& ps = mParticleSystem.getParticles();
        auto rad = 1.0f;
        auto scale = Math::Util::scaleMatrix3D({rad, rad, rad});
        for (int i = 0; i < mParticleSystem.getParticleCount(); i++)
        {
            auto rotMatrix = Math::Quatrenion::angleToQuat(rots[i]).getRotationMatrix()
                * scale;
            auto mvp = proj * mShuttles[mCurrentShuttle].getMatrix() * 
                Math::Util::transMatrix(rotMatrix, ps[i].getPos());
            mRenderer->updatePushConstant(&mvp, 0);
            mRenderer->render();
        }
        // End Pass PS
        /* 
           mRenderer->bindShader(3);
           rad = 400.0f;
           scale = Math::Util::scaleMatrix3D({rad, 0.1f, rad});
           auto trans = 
           proj * mShuttle.getMatrix() * 
           Math::Util::transMatrix(Math::Util::identityMatrix() * scale, 
           mTransformComponents["planet"].getPosition());
           mRenderer->updatePushConstant(&trans, 0);
           mRenderer->render();

           auto spherec = Math::Vector3D(mTraceData.sphereOrg.x, mTraceData.sphereOrg.y,
           mTraceData.sphereOrg.z);
           */

        /*
           mRenderer->render();
           */

        //mRenderer->endPass();


        mRenderer->bindShader(4);
        mRenderer->updatePushConstant(&mTraceData, 0);
        mRenderer->submitUniformUpdates();
        mRenderer->nextPass();
        mRenderer->submitUniformUpdates();
        mRenderer->render();


        mRenderer->endPass();
        mRenderer->present();

        // post process
        
        // end posprocess

        // End Passing System


        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }

}}

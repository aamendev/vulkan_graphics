#include "final.h"

namespace Lina { namespace Games {

    void FinalGameWorld::preinit()
    {   
        planetLayers.push_back({
                OCEAN,
                GREEN,
                DIRT
                });
        GameSystem::GameObject firstPlayer("p1");    
        GameSystem::GameObject planet("planet");
        GameSystem::GameObject losingFloor("lose");

        ECS::CollisionSystem* cSystem = new ECS::CollisionSystem();
        addCollisionSystem(cSystem);

        firstPlayer.addComponent(ComponentType::Transform);
        firstPlayer.addComponent(ComponentType::Health);
        firstPlayer.addComponent(ComponentType::CylinderCollider);
        firstPlayer.addComponent(ComponentType::Material);


        planet.addComponent(ComponentType::Transform);
        planet.addComponent(ComponentType::MeshCollider);
        planet.addComponent(ComponentType::Material);

        losingFloor.addComponent(ComponentType::Transform);
        losingFloor.addComponent(ComponentType::PlaneCollider);
        losingFloor.addComponent(ComponentType::Material);
        losingFloor.removeComponent(ComponentType::Material);

        addObject(firstPlayer);
        addObject(planet);
        addObject(losingFloor);

        fetchComponents(); 
        for (auto& [tag, value] : mCylinderColliderComponents)
        {
            value.setCallDefaults(false);
            cSystem->addCollider(&value);
        }
        f32 rad = 200.0f;
        mMeshColliderComponents["planet"].setScale({rad, rad, rad});
        mMeshColliderComponents["planet"].setVertices(mEnvVerts);
        mMeshColliderComponents["planet"].setIndices(mEnvInds);
        mTransformComponents["planet"].setScale({rad, rad, rad});

        for (auto& [_, value] : mPlaneColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }
        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }


        cSystem->staticOptimize();
        rad = 600;
        //cSystem->constructGrid({-rad, -rad, -rad}, {rad, rad, rad}, 50);
        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();

        rad = 220.0f;
        mParticleSystem.setMaxLifeTime(10.0f);
        mParticleSystem.setRadius(rad);
        mParticleSystem.setParticleCount(200);
        mParticleSystem.setMinRotation({0, 0, 0});
        mParticleSystem.setMaxRotation({0, 0, PI/8});
        mParticleSystem.setMinV({2.0f, 6.0f, 0.0f});
        mParticleSystem.setMaxV({4.0f, 20.0f, 0.0f});
    }


    void FinalGameWorld::setCallBacks()
    {
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                auto* d = (ECS::Components::Colliders::Mesh*)c2;
                auto inf = d->getInfo();
                mUp = inf.normal;
                if (c2->getTag() == "planet")
                {
                    onSlope = true;
                    mCharacterControllers[0]->setGrounded(true);
                    if (!physicsMode)
                    {
                        slopeGravity = inf.normal * 0.01f;
                        mCharacterControllers[0]->setYVelocity(0);

                        if (inf.depth > 0.1f)
                        {
                            mTransformComponents[c1->getTag()].translate(inf.normal * inf.depth);
                        }

                    }
                    else 
                    {
                        auto restDir = (playerRb.getPos() - sunParticle.getPos()).normalise();
                        auto rest = (playerRb.getLinearVelocity().dot(
                                    restDir
                                    )) * mFrameRate * playerRb.getMass();
                        auto restForce = -restDir * rest;
                        playerRb.applyImpulse(restForce);
                        //playerRb.disableForce(0);
                        playerRb.inContact();
                    }
                }
                else if (c2->getTag() == "lose")
                {
                    reset();
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionPersist = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                if (c2->getTag() == "planet")
                {
                    static u32 collisionCount = 0;
                    auto* d = (ECS::Components::Colliders::Mesh*)c2;
                    auto inf = d->getInfo();
                    mUp = inf.normal;
                    if (physicsMode)
                    {
                        auto restDir = (playerRb.getPos() - sunParticle.getPos()).normalise();
                        auto rest = (playerRb.getLinearVelocity().dot(
                                    restDir
                                    )) * mFrameRate * playerRb.getMass();
                        auto restForce = -restDir * rest;

                        playerRb.applyImpulse(restForce);

                        auto netForce = 
                            (playerRb.getLinearAcceleration() * playerRb.getMass());
                        auto upForce = netForce.dot(mUp);
                        auto muk = 0.8f;
                        auto moveDir = playerRb.getLinearVelocity();

                        auto ff = moveDir.normalise() * (-muk * upForce);
                        auto mag = moveDir.squaredMagnitude();
                        ff *= (mag > 0.0f);
                        playerRb.applyImpulse(ff);
                        auto rebel = mUp * upForce;
                        playerRb.applyImpulse(rebel);
                        /*if (inf.depth > 0.1f)
                          {
                          auto updatePos = (inf.normal * inf.depth).toPoint();
                          playerRb.updatePosition(std::move(updatePos));
                          }*/
                    }
                    // contact:

                    else 
                    {

                        auto inf = d->getInfo();
                        {
                            mTransformComponents[c1->getTag()].translate(inf.normal * 
                                    (inf.depth));
                        }
                    }
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionExit = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{

                if(c2->getTag() == "planet") 
                {
                    slopeGravity = {0, 0, 0};
                    onSlope = false;
                    mCharacterControllers[0]->setGrounded(false);
                    mUp = (c2->getPosition() - c1->getPosition()).normalise();
                    //mUp = {0,1,0};
                    if (physicsMode)
                    {
                        playerRb.enableForce(0);
                        playerRb.notInContact();
                    }
                }
            };


        mCylinderColliderComponents["p1"].setOnCollisionEnter(
                playerCollisionEnter);

        mCylinderColliderComponents["p1"].setOnCollisionPersist(
                playerCollisionPersist);
        mCylinderColliderComponents["p1"].setOnCollisionExit(playerCollisionExit);
    }

    void FinalGameWorld::init()
    {
        mParticleSystem.init();
        translationRate = 0.2f * !physicsMode + 3.2f * physicsMode; 
        mJump = 1.2f * !physicsMode + 200.0f * physicsMode; 
        mCylinderColliderComponents["p1"].setPosition({0, -150, 0});
        mCylinderColliderComponents["p1"].setHeight(2);
        mCylinderColliderComponents["p1"].setRadius(1);
        mCylinderColliderComponents["p1"].setCallDefaults(false);
        mCharacterControllers[0]->setVelocity({0, 0, 0});
        mCharacterControllers[0]->setRotationVelocity({0, 0, 0});

        mTransformComponents["p1"].setPosition({0, -150, 0});
        mTransformComponents["p1"].setScale({1, 2, 1});

        mMaterialComponents["p1"].setColour({(float)0x73 / 0xff, 
                (float)0x87 / 0xff, (float)0x57 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["p1"].setShader(0);

        // mMaterialComponents["planet"].setColour({(float)0x6B / 0xff, 
        //        (float)0x57 / 0xff, (float)0x87 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["planet"].setDynamic(true);
        for (auto& c : planetLayers[0])
        {
            mMaterialComponents["planet"].addColour(c);
        }
        mMaterialComponents["planet"].setShader(1);

        mTransformComponents["lose"].disable();
        mPlaneColliderComponents["lose"].setLength(2000);
        mPlaneColliderComponents["lose"].setWidth(2000);
        mPlaneColliderComponents["lose"].setPosition({0, 200, 0});


        //PHYSICS INIT
        auto scale = mTransformComponents["p1"].getScale();
        auto playerPos = mTransformComponents["p1"].getPosition();
        Physics::Particle* playerParticle = new Physics::Particle();
        mUp = (mTransformComponents["planet"].getPosition() 
                - mTransformComponents["p1"].getPosition()).normalise();
        playerParticle->setMass(0.2f);
        playerParticle->setPosition(std::move(playerPos));
        Physics::RbSetting rb =
        {
            .particle = playerParticle,
        };
        auto fac1 = (1.0f / playerParticle->getInverseMass()) * scale.x * scale.x / 4;
        auto fac2 = 
            ((1.0f / playerParticle->getInverseMass()) * scale.y * scale.y / 12) +
            fac1;
        Math::Matrix3D intTensor = 
        {
            fac2, 0, 0,
            0, fac2, 0,
            0, 0, fac1
        };
        rb.invInertiaTensor = intTensor.inverse();
        playerRb.loadSetting(rb);

        sunParticle.setMass(200);
        sunParticle.setPosition({0,0,0});

        Physics::Forces::Gravity* gravForce = new Physics::Forces::Gravity;
        gravForce->addBody(playerParticle);
        gravForce->addBody(&sunParticle);
        gravForce->setG(7);
        playerRb.applyForce(gravForce, {0, 0, 0});
    }

    void FinalGameWorld::reset()
    {
        init();
    }

    void FinalGameWorld::onKeyUp(Events::KeyRelease& e)
    {
        auto& v = mCharacterControllers[0]->getVelocity();
        switch(e.key())
        {
            case Input::KeyCode::W:
                {
                    zMove--;
                }
                break;

            case Input::KeyCode::S:
                {
                    zMove++;
                }
                break;

            case Input::KeyCode::D:
                {
                    xMove--;                
                }
                break;

            case Input::KeyCode::A:
                {
                    xMove++;
                }
                break;
            case Input::KeyCode::Space:
                {
                    isJump = false;
                }
                break;
            case Input::KeyCode::R:
                reset();
                break;
            case Input::KeyCode::Left:
                {
                    mCharacterControllers[0]->setRotationVelocity({0, 0.0f,0});
                }
                break;
            case Input::KeyCode::Right:
                {
                    mCharacterControllers[0]->setRotationVelocity({0, -0.0f,0});
                }
                break;
            default:{}
        }
    }

    void FinalGameWorld::onKeyDown(Events::KeyPress& e)
    {
        auto& v = mCharacterControllers[0]->getVelocity();
        switch(e.key())
        {
            case Input::KeyCode::W:
                {
                    zMove = 1;
                }
                break;

            case Input::KeyCode::S:
                {
                    zMove = -1;
                }
                break;

            case Input::KeyCode::A:
                {
                    xMove = -1;
                }
                break;

            case Input::KeyCode::D:
                {
                    xMove = 1;
                }
                break;
            case Input::KeyCode::Space:
                {
                    if (mCharacterControllers[0]->isGrounded())
                    {
                        isJump = true;
                        mCharacterControllers[0]->setGrounded(false);
                    }
                }
                break;
            case Input::KeyCode::Left:
                {
                    mCharacterControllers[0]->setRotationVelocity({0, 0.1f,0});
                }
                break;
            case Input::KeyCode::Right:
                {
                    mCharacterControllers[0]->setRotationVelocity({0, -0.1f,0});
                }
                break;

            default:{}
        }
    }
    void FinalGameWorld::move()
    {
        auto oldv = mCharacterControllers[0]->getVelocity();
        auto dirz = Math::Util::zAxis().reject(mUp);
        auto newvz = (dirz.normalise() * translationRate) * (zMove);

        auto dirx = Math::Util::xAxis().reject(mUp);
        auto newvx = (dirx.normalise() * translationRate) * (xMove);

        auto newV = newvx + newvz + slopeGravity * onSlope
            + mUp * (oldv.dot(mUp)  * !onSlope) * !physicsMode;

        if (physicsMode)
        {
            playerRb.applyImpulse(newV);
            auto j = mUp * (-mJump * isJump);
            playerRb.applyImpulse(j);
            playerRb.update(1/mFrameRate);
            mTransformComponents["p1"].setPosition(playerRb.getPos());
            mCylinderColliderComponents["p1"].setPosition(playerRb.getPos());
        }
        else 
        {
            mCharacterControllers[0]->setVelocity(newV);
            mCharacterControllers[0]->updateVelocity(mUp * (-mJump * isJump));
        }
        isJump = false;
    }
    void FinalGameWorld::update()
    {
        mTimer.begin();
        auto followPos = mTransformComponents["p1"].getPosition();
        auto zoom = 40.0f;
        mShuttle.setPosition(followPos.x, followPos.y - 3, followPos.z - zoom);
        auto& pos = mShuttle.getPos();
        auto& dir = mShuttle.getTarget();
        auto& up = mShuttle.getUp();
        mTraceData.rayOrg = Math::Vector4D(pos.x, pos.y, pos.z, 0.0f);
        mTraceData.rayDirection = Math::Vector4D(dir.x, dir.y, dir.z, 0.0f);
        mTraceData.rayUp = Math::Vector4D(up.x, up.y, up.z, 0.0f);
        for (auto& col : mCollisionSystems)
            col->update();

        move();
        for (auto& [key, _] : mPlaneColliderComponents)
        {
            auto vv = 
                (mPlaneColliderComponents[key].getPosition()); 
            mTransformComponents[key].setPosition(vv);
        }
        for (auto& [key, _] : mMeshColliderComponents)
        {
            mMeshColliderComponents[key].setPosition(
                    mTransformComponents[key].getPosition().toPoint());
        }

        for (auto& cc : mCharacterControllers)
            cc->update();

        for (auto& [key, _] : mCylinderColliderComponents)
        {
            mCylinderColliderComponents[key].setPosition(
                    mTransformComponents[key].getPosition());
        }

        for (auto& [key, _] : mPlaneColliderComponents)
        {
            auto vv = 
                (mTransformComponents[key].getPosition());
            mPlaneColliderComponents[key].setPosition(vv);
        }
        if (!mCharacterControllers[0]->isGrounded() && !physicsMode)
        {
            mCharacterControllers[0]->updateVelocity({0, mGravity, 0});
        }

        mParticleSystem.update(1/mFrameRate);
        //Begin Pass System

        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);


        mRenderer->beginPasses();
        mRenderer->setPrimitive(Primitive::Triangle);

        for (auto& [key, value] : mMaterialComponents)
        {
            if (value.isEnabled())
            {
                mRenderer->bindShader(value.getShaderId());
                if (value.getShaderId() != 1)
                {
                    if (mTransformComponents[key].isEnabled())
                    {
                        auto currentTransMat = proj * mShuttle.getMatrix() 
                            * mTransformComponents[key].getMatrix()
                            * Math::Util::scaleMatrix(mTransformComponents[key].getScale()); 
                        mRenderer->updatePushConstant(&currentTransMat, 0);
                    }
                    auto col = value.getColour();
                    mRenderer->updateUniform(&col, 0, 0);
                    mRenderer->submitUniformUpdates();
                }
                else 
                {
                    auto currentTransMat = mTransformComponents[key].getMatrix()
                        * Math::Util::scaleMatrix(mTransformComponents[key].getScale()); 
                    auto projMatrix = proj * mShuttle.getMatrix();
                    std::array<Math::Matrix4D, 2> ps = {currentTransMat, projMatrix};

                    mRenderer->updatePushConstant(ps.data(), 0);
                    auto* col = value.getMultipleColoursPointer();
                    mRenderer->updateUniform(&mLightData, 0);
                    mRenderer->updateUniform((void*)col, 1, 0);
                    mRenderer->submitUniformUpdates();
                }
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
        auto rad = 10.0f;
        auto scale = Math::Util::scaleMatrix3D({rad, rad, rad});
        for (int i = 0; i < mParticleSystem.getParticleCount(); i++)
        {
            auto rotMatrix = Math::Quatrenion::angleToQuat(rots[i]).getRotationMatrix()
                * scale;
            auto mvp = proj * mShuttle.getMatrix() * 
                Math::Util::transMatrix(rotMatrix, ps[i].getPos());
            mRenderer->updatePushConstant(&mvp, 0);
            mRenderer->render();
        }
        // End Pass PS
        
        mRenderer->bindShader(3);
        rad = 400.0f;
        scale = Math::Util::scaleMatrix3D({rad, 0.1f, rad});
        auto trans = 
            proj * mShuttle.getMatrix() * 
            Math::Util::transMatrix(Math::Util::identityMatrix() * scale, 
                mTransformComponents["planet"].getPosition());
        mRenderer->updatePushConstant(&trans, 0);
        mRenderer->render();

        /*
           mRenderer->render();
           */

        //mRenderer->endPass();

        mRenderer->bindShader(4);
        mRenderer->updatePushConstant(&mTraceData, 0);
        mRenderer->submitUniformUpdates();
        mRenderer->nextPass();
        mRenderer->submitUniformUpdates();
        //mRenderer->loadMainTexture();
        mRenderer->render();

        mRenderer->endPass();
        mRenderer->present();
        
        // post process
        /*
        mRenderer->beginPass();
        mRenderer->setPrimitive(Primitive::Triangle);
        mRenderer->endPass();
        */
        // end posprocess

        // End Passing System
        

        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }

}}

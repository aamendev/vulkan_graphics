#include "collision_world.h"
#include <cstdlib>
#include <string>

namespace Lina { namespace Games {

    void CollisionWorld::preinit()
    {   
        GameSystem::GameObject firstPlayer("p1");    

        firstPlayer.addComponent(ComponentType::Transform);
        firstPlayer.addComponent(ComponentType::CylinderCollider);
        firstPlayer.addComponent(ComponentType::Material);

        addObject(firstPlayer);

        GameSystem::GameObject token("token");    

        token.addComponent(ComponentType::Transform);
        token.addComponent(ComponentType::CylinderCollider);
        token.addComponent(ComponentType::Material);
        addObject(token);

        GameSystem::GameObject Environment("env");    
        Environment.addComponent(ComponentType::Transform);
        Environment.addComponent(ComponentType::MeshCollider);
        Environment.addComponent(ComponentType::Material);

        addObject(Environment);

        GameSystem::GameObject losingFloor("lose");

        losingFloor.addComponent(ComponentType::Transform);
        losingFloor.addComponent(ComponentType::PlaneCollider);
        addObject(losingFloor);


        fetchComponents(); 

        ECS::CollisionSystem* cSystem = 
            new ECS::CollisionSystem();
        addCollisionSystem(cSystem);
        for (auto& [tag, value] : mCylinderColliderComponents)
        {
            if (tag == "p1")
            {
            cSystem->addCollider(&value, ColliderType::Dynamic);
            }
            else {
            cSystem->addCollider(&value, ColliderType::Static);
            }
        }

        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }

        for (auto& [_, value] : mPlaneColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }
        auto rad = 5.0f;
        mMeshColliderComponents["env"].setVertices(mEnvVerts[0]);
        mMeshColliderComponents["env"].setPosition({0, 0 , 0});
        mMeshColliderComponents["env"].setScale({rad, rad, rad});
        mMeshColliderComponents["env"].setRotation({0, 0, PI});

        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();

        mShuttle.setPosition(0, 0, -20);
        mShuttle.setSpeed(2);

        mParticleSystem.setMaxLifeTime(15.0f);
        mParticleSystem.setRadius(10);
        mParticleSystem.setParticleCount(10);
        mParticleSystem.setMinRotation({0, 0, 0});
        mParticleSystem.setMaxRotation({0, 0, 0});
        mParticleSystem.setMinV({1.0f, 5.0f, 0.0f});
        mParticleSystem.setMaxV({1.5f, 10.0f, 0.0f});
        mParticleSystem.setPosition({0, -70, 0});


        f32 dy = -10;
        f32 dx = 10;
        f32 dz = 10;
        u32 rampCount = 1;
        Math::Point3D curr = {0, -47, 10};
        for (int i = 0; i < rampCount; i++)
        {
            curr += ((Math::Vector3D){dx, dy, dz} * (i != 0));
            addRamp(std::move(curr), {PI, 0.125f * PI * i, 0});
        }
        
        curr += (Math::Vector3D){0, -3, 0};
        mTransformComponents["token"].setPosition(curr);
        mCylinderColliderComponents["token"].setPosition(curr);

        rad = 1.0f;
        mCylinderColliderComponents["token"].setRotation({0, 0, 0});
        mCylinderColliderComponents["token"].setHeight(rad);
        mCylinderColliderComponents["token"].setRadius(rad);

        mTransformComponents["token"].setScale({rad, rad, rad});
        mTransformComponents["token"].setRotation({0, 0, 0});


        mMaterialComponents["token"].setColour({(float)0x22 / 0xff, 
                (float)0xbf / 0xff, (float)0x04 / 0xff, (float)0xff / 0xff});

        mMaterialComponents["token"].setShader(0);

        mParticleSystem.init();

        mTransformComponents["lose"].disable();
        mTransformComponents["lose"].setPosition({0, 200, 0});
        mPlaneColliderComponents["lose"].setLength(20000);
        mPlaneColliderComponents["lose"].setWidth(20000);
        mPlaneColliderComponents["lose"].setPosition({0, 200, 0});
        addPSColliders();
       cSystem->optimize();
       cSystem->constructGrid({-100, -150, -100}, {100, 100, 100}, 10);
    }
    void CollisionWorld::addPSColliders()
    {
        auto& ps = mParticleSystem.getParticles();
        auto& rots = mParticleSystem.getRotations();
        for (int i = 0; i <  mParticleSystem.getParticleCount(); i++) 
        {
            std::string partTag = "part" + std::to_string(i);
            GameSystem::GameObject part(partTag);    
            part.addComponent(ComponentType::CylinderCollider);
            part.addComponent(ComponentType::Material);
            part.addComponent(ComponentType::Transform);
            addObject(part);
            fetchComponents(); 
            mCollisionSystems[0]->addCollider(&mCylinderColliderComponents[partTag],
                    ColliderType::Dynamic);
         mCylinderColliderComponents[partTag].setPosition(ps[i].getPos());
         mCylinderColliderComponents[partTag].setRadius(2);
         mCylinderColliderComponents[partTag].setHeight(2);
         mCylinderColliderComponents[partTag].setRotation(rots[i]);

            mTransformComponents[partTag].setPosition(ps[i].getPos());
        mTransformComponents[partTag].setScale({2, 2, 2});
        mTransformComponents[partTag].setRotation({0, 0, 0});


        mMaterialComponents[partTag].setColour({(float)0x39 / 0xff, 
                (float)0x49 / 0xff, (float)0xff / 0xff, (float)0xff / 0xff});
        mMaterialComponents[partTag].setShader(1);
        }
    }
    void CollisionWorld::addRamp(Math::Point3D&& p, Math::EulerAngles&& rot)
    {
        std::string rampTag = "ramp" + std::to_string(mRampCount);
        GameSystem::GameObject ramp(rampTag);    

        ramp.addComponent(ComponentType::Transform);
        ramp.addComponent(ComponentType::MeshCollider);
        ramp.addComponent(ComponentType::Material);
        addObject(ramp);

        auto boundTag = "rampBound" + std::to_string(mRampCount);

        GameSystem::GameObject boundingBox(boundTag);
        boundingBox.addComponent(ComponentType::Transform);
        boundingBox.addComponent(ComponentType::Material);
        addObject(boundingBox);

        fetchComponents(); 
        mCollisionSystems[0]->addCollider(&mMeshColliderComponents[rampTag],
                ColliderType::Static);

        auto r = 1.5f;
        mMeshColliderComponents[rampTag].setPosition(p);
        mMeshColliderComponents[rampTag].setScale({r, r, r});
        mMeshColliderComponents[rampTag].setVertices(mEnvVerts[1]);
        mMeshColliderComponents[rampTag].setRotation(rot);
        //mMeshColliderComponents[rampTag].computeBoundingBox();

        mTransformComponents[rampTag].setPosition(p);
        mTransformComponents[rampTag].setScale({r, r, r});
        mTransformComponents[rampTag].setRotation(rot);

        mMaterialComponents[rampTag].setColour({(float)0xef / 0xff, 
                (float)0x00 / 0xff, (float)0x04 / 0xff, (float)0xff / 0xff});
        mMaterialComponents[rampTag].setShader(2);

        mRampCount++;
    }

    void CollisionWorld::reset()
    {
        init();
    }
    void CollisionWorld::setCallBacks()
    {
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                b8 isRamp = c2->getTag().find("ramp") != std::string::npos;
                b8 isPS = c2->getTag().find("part") != std::string::npos;

                if ((c2->getTag() == "env") || isRamp)
                {
                    auto* d = (ECS::Components::Colliders::Mesh*)c2;
                    auto inf = d->getInfo();
                    mUp = inf.normal;
                    slopeGravity = inf.normal * 0.01f;
                    onSlope = true;
                    mCharacterControllers[0]->setGrounded(true);
                    mCharacterControllers[0]->setYVelocity(0);
                    if (inf.depth > 0.1f)
                    {
                    mTransformComponents[c1->getTag()].translate(inf.normal * inf.depth);
                    }
                }
                else if (isPS) 
                {
                    std::cerr << "DYNAMICEnter\n";
                    mCollectedCoins++;
                    mTransformComponents[c2->getTag()].disable();
                    mCylinderColliderComponents[c2->getTag()].disable();
                }
                else if (c2->getTag() == "lose")
                {
                    std::cerr << "REAET\n";
                    reset();
                }
                else if (c2->getTag() == "token")
                {
                    reset();
                    won = true;
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionPersist = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{

                c1->setCallDefaults(false);
                b8 isRamp = c2->getTag().find("ramp") != std::string::npos;
                b8 isPS = c2->getTag().find("part") != std::string::npos;
               // std::cerr << "Persist\n";
                if (c2->getTag() == "env" || isRamp)
                {
                    static u32 collisionCount = 0;
                    auto* d = (ECS::Components::Colliders::Mesh*)c2;
                    auto inf = d->getInfo();
                    {
                        mTransformComponents[c1->getTag()].translate(inf.normal * 
                                (inf.depth));
                    }
                }

                else if (c2->getTag() == "token")
                {
                    std::cerr << "GOt Token!\n";
                    reset();
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionExit = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                auto* d1 = dynamic_cast<ECS::Components::Colliders::Cylinder*>(c1);
               // std::cerr << "EndedCollision\n";

                b8 isRamp = c2->getTag().find("ramp") != std::string::npos;
                b8 isPS = c2->getTag().find("part") != std::string::npos;
                if(c2->getTag() == "env" || isRamp) 
                {
                    slopeGravity = {0, 0, 0};
                    onSlope = false;
                    mCharacterControllers[0]->setGrounded(false);
                    mUp = {0, 1, 0};
                }
            };
        mCylinderColliderComponents["p1"].setOnCollisionEnter(
                playerCollisionEnter);

        mCylinderColliderComponents["p1"].setOnCollisionPersist(
                playerCollisionPersist);
        mCylinderColliderComponents["p1"].setOnCollisionExit(playerCollisionExit);
    }

    void CollisionWorld::init()
    {
        won = false; 
        mCylinderColliderComponents["p1"].setPosition({0, -50 ,0});
        mCylinderColliderComponents["p1"].setHeight(2);
        mCylinderColliderComponents["p1"].setRadius(1);
        mCylinderColliderComponents["p1"].setRotation({0, 0, 0});

       // mTransformComponents["p1"].setPosition({2.35454, -44.6821, 6.74587});
        mTransformComponents["p1"].setPosition({0, -47, 0});
        mTransformComponents["p1"].setScale({1, 2, 1});
        mTransformComponents["p1"].setRotation({0, 0, 0});

        mMaterialComponents["p1"].setColour({(float)0x73 / 0xff, 
                (float)0x87 / 0xff, (float)0x57 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["p1"].setShader(0);


        auto rad = 5.0f;

        mTransformComponents["env"].setPosition({0, 0, 0});
        mTransformComponents["env"].setScale({rad, rad, rad});
        mTransformComponents["env"].setRotation({0, 0, PI});

        mMaterialComponents["env"].setColour({(float)0xef / 0xff, 
                (float)0xbf / 0xff, (float)0x04 / 0xff, (float)0xff / 0xff});

        mMaterialComponents["env"].setShader(3);
    }

    void CollisionWorld::move()
    {
        auto oldv = mCharacterControllers[0]->getVelocity();
        auto dirz = Math::Util::zAxis().reject(mUp);
        auto newvz = (dirz.normalise() * translationRate) * (zMove);

       auto dirx = Math::Util::xAxis().reject(mUp);
        auto newvx = (dirx.normalise() * translationRate) * (xMove);

        auto newV = newvx + newvz + slopeGravity * onSlope
                + (Math::Vector3D){0, oldv.y, 0} * !onSlope;
        mCharacterControllers[0]->setVelocity(newV);
        mCharacterControllers[0]->updateVelocity({0, -mJump * isJump, 0});
        isJump = false;
    }

    void CollisionWorld::onKeyDown(Events::KeyPress& e)
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

            default:{}
        }
    }


    void CollisionWorld::onKeyUp(Events::KeyRelease& e)
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
            default:{}
        }
    }
    void CollisionWorld::update()
    {
        mTimer.begin();
        if (!won && mCollectedCoins < 10)[[likely]]
        {
           std::cerr << "CurrentCoins: " << mCollectedCoins << '\n';
        for (auto& col : mCollisionSystems)
            col->update();

        move();

        for (auto& cc : mCharacterControllers)
            cc->update();


        for (auto& [key, _] : mCylinderColliderComponents)
        {
            mCylinderColliderComponents[key].setPosition(
                    mTransformComponents[key].getPosition().toPoint());
        }

        for (auto& [key, _] : mMeshColliderComponents)
        {
            mMeshColliderComponents[key].setPosition(
                    mTransformComponents[key].getPosition().toPoint());
        }

        for (auto& [key, _] : mPlaneColliderComponents)
        {
            mMeshColliderComponents[key].setPosition(
                    mTransformComponents[key].getPosition().toPoint());
        }

        if (!mCharacterControllers[0]->isGrounded() * !onSlope)
        {
            mCharacterControllers[0]->updateVelocity({0, mGravity, 0});
        }

        }
        //Begin Draw System

        typedef struct color 
        {
            f32 r = 0.0f;
            f32 g = 1.0f;
            f32 b = 1.0f;
            f32 a = 1.0f;
        } Col;

        //Col col;

        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);

        mParticleSystem.update(1.0f / 60);
        auto& ps = mParticleSystem.getParticles();
        auto& rots = mParticleSystem.getRotations();
       /* for (int i = 0; i < mParticleSystem.getParticleCount(); i++)
        {
            auto partTag = "part" + std::to_string(i);
             mCylinderColliderComponents[partTag].setPosition(ps[i].getPos());
            mCylinderColliderComponents[partTag].setScale({1, 1, 1});
            mCylinderColliderComponents[partTag].setRotation(rots[i]);
        }*/

        mRenderer->beginDraw();
        mRenderer->bindShader(0);
        mRenderer->setPrimitive(Primitive::Triangle);
      //  mRenderer->setPrimitive(Primitive::LineList);

        for (auto& [key, value] : mMaterialComponents)
        {
            if (value.isEnabled() && mTransformComponents[key].isEnabled())
            {
                mRenderer->bindShader(value.getShaderId());
                    auto currentTransMat = proj * mShuttle.getMatrix() 
                        * mTransformComponents[key].getMatrix()
                        * Math::Util::scaleMatrix(mTransformComponents[key].getScale()); 
                    mRenderer->updatePushConstant(&currentTransMat, 0);
                auto col = value.getColour();

                b8 isPS = key.find("part") != std::string::npos;
                mRenderer->updateUniform(&col, 0, key == "token");
                mRenderer->render();
            }
        }

        // Draw Particle System
        mRenderer->bindShader(1);
        auto scale = Math::Util::scaleMatrix({0.3f, 1.5f, 0.3f});
        auto col = mMaterialComponents["part1"].getColour();
        mRenderer->updateUniform(&(col), 0, 1);
        for (int i = 0; i < ps.size(); i++)
        {
                auto partTag = "part" + std::to_string(i);
            mTransformComponents[partTag].setPosition(ps[i].getPos());
            mTransformComponents[partTag].setRotation(rots[i]);
           /* 

           auto trans = proj * mShuttle.getMatrix() *
               Math::Util::transMatrix(Math::Util::identityMatrix(), 
                   ps[i].getPos()) 
               * Math::Util::transMatrix((Math::Quatrenion::angleToQuat(rots[i])).getRotationMatrix(),
                        {0, 0, 0}) * scale; 

            mRenderer->updatePushConstant(&trans, 0);
            mRenderer->render();*/
        }

        // End Draw Particle System
        //

        auto followPos = mTransformComponents["p1"].getPosition();
        mShuttle.setPosition(followPos.x, followPos.y, followPos.z - 10);
        mRenderer->endDraw();
        // End Drawing System

        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }

}}

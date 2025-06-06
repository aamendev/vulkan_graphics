#include "warmup2.h"
#include <cstdlib>

namespace Lina { namespace Games {
    
    void W2GameWorld::preinit()
    {   
        GameSystem::GameObject firstPlayer("p1");    
        GameSystem::GameObject secondPlayer("p2");
        GameSystem::GameObject plane("plane");
        GameSystem::GameObject losingFloor("lose");

        ECS::CollisionSystem* cSystem = new ECS::CollisionSystem();
        addCollisionSystem(cSystem);

        firstPlayer.addComponent(ComponentType::Transform);
        firstPlayer.addComponent(ComponentType::Health);
        firstPlayer.addComponent(ComponentType::CylinderCollider);
        firstPlayer.addComponent(ComponentType::Material);

        secondPlayer.addComponent(ComponentType::Transform);
        secondPlayer.addComponent(ComponentType::Health);
        secondPlayer.addComponent(ComponentType::CylinderCollider);
        secondPlayer.addComponent(ComponentType::Material);

        plane.addComponent(ComponentType::Transform);
       // plane.addComponent(ComponentType::PlaneCollider);
        plane.addComponent(ComponentType::MeshCollider);
        plane.addComponent(ComponentType::Material);

        losingFloor.addComponent(ComponentType::Transform);
        losingFloor.addComponent(ComponentType::PlaneCollider);
        losingFloor.addComponent(ComponentType::Material);
        losingFloor.removeComponent(ComponentType::Material);

        addObject(firstPlayer);
        addObject(secondPlayer);
        addObject(plane);
        addObject(losingFloor);

        fetchComponents(); 
        for (auto& [_, value] : mCylinderColliderComponents)
        {
            cSystem->addCollider(&value);
        }

        for (auto& [_, value] : mPlaneColliderComponents)
        {
            cSystem->addCollider(&value);
        }
        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }

        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();
    }


    void W2GameWorld::setCallBacks()
    {
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                if (c2->getTag() == "plane")
                {
                    mCharacterControllers[0]->setGrounded(true);
                    mCharacterControllers[0]->setYVelocity(0);
                }else if (c2->getTag() == "p2")
                {
                    auto* d = (ECS::Components::Colliders::Cylinder*)c2;
                    d->setCallDefaults(false);
                    d->setPosition({(float) (rand() % 800) - 400, 0, (float)(rand() % 800) - 400});
                    mCoinCount++;
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

                if (c2->getTag() == "plane")
                {
                    mCharacterControllers[0]->setGrounded(true);
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionExit = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                auto* d1 = dynamic_cast<ECS::Components::Colliders::Cylinder*>(c1);
                if(c2->getTag() == "plane") mCharacterControllers[0]->setGrounded(false);
            };


        mCylinderColliderComponents["p1"].setOnCollisionEnter(
                playerCollisionEnter);

        mCylinderColliderComponents["p1"].setOnCollisionPersist(
                playerCollisionPersist);
        mCylinderColliderComponents["p1"].setOnCollisionExit(playerCollisionExit);

    }

    void W2GameWorld::init()
    {
        mCoinCount = 0;
        mCylinderColliderComponents["p1"].setPosition({40, -15 ,40});
        mCylinderColliderComponents["p1"].setHeight(10);
        mCylinderColliderComponents["p1"].setRadius(10);
        mCharacterControllers[0]->setVelocity({0, 0, 0});

        mTransformComponents["p1"].setPosition({40, -15, 40});
        mTransformComponents["p1"].setScale({20, 20, 10});

        mMaterialComponents["p1"].setColour({(float)0x73 / 0xff, 
                (float)0x87 / 0xff, (float)0x57 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["p1"].setShader(0);

        mTransformComponents["p2"].setScale({20, 20, 10});
        mMaterialComponents["p2"].setColour(
                {(float)0xaa / 0xff,
                (float)0xff / 0xff,
                (float)0xff / 0xff,
                (float)0xff / 0xff}
                );

        mMaterialComponents["p2"].setShader(0);
        mCylinderColliderComponents["p2"].setHeight(10);
        mCylinderColliderComponents["p2"].setRadius(10);
        mTransformComponents["p2"].setPosition({0, -8, 0});
        mCylinderColliderComponents["p2"].setPosition({0, 0 ,0});

       // mPlaneColliderComponents["plane"].setLength(1000);
        //mPlaneColliderComponents["plane"].setWidth(1000);
        mTransformComponents["plane"].setScale({50, 50, 50});
        mTransformComponents["plane"].setRotation({0, 0, PI});
        
        mMeshColliderComponents["plane"].setVertices(mEnvVertices);
        mMeshColliderComponents["plane"].setScale({50, 50, 50});
        mMeshColliderComponents["plane"].setRotation({0,0, PI});
        mMaterialComponents["plane"].setColour({(float)0x6B / 0xff, 
                (float)0x57 / 0xff, (float)0x87 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["plane"].setShader(1);

        mTransformComponents["lose"].disable();
        mPlaneColliderComponents["lose"].setLength(2000);
        mPlaneColliderComponents["lose"].setWidth(2000);
        mPlaneColliderComponents["lose"].setPosition({0, 200, 0});
    }

    void W2GameWorld::reset()
    {
        init();
    }

    void W2GameWorld::onKeyUp(Events::KeyRelease& e)
    {
        auto& v = mCharacterControllers[0]->getVelocity();
        switch(e.key())
        {
            case Input::KeyCode::W:
                mCharacterControllers[0]->setVelocity({v.x, v.y, 0});
               break;

            case Input::KeyCode::S:
                mCharacterControllers[0]->setVelocity({v.x, v.y, 0});
               break;

            case Input::KeyCode::D:
                mCharacterControllers[0]->setVelocity({0, v.y, v.z});
               break;

            case Input::KeyCode::A:
                mCharacterControllers[0]->setVelocity({0, v.y, v.z});
               break;
            case Input::KeyCode::Space:
               {
                   if (mCharacterControllers[0]->isGrounded())
                   {
                mCharacterControllers[0]->updateVelocity({0, -mJump, 0});
                mCharacterControllers[0]->setGrounded(false);
                   }
               }
            break;
            case Input::KeyCode::R:
                reset();
               break;
               default:{}
        }
    }

    void W2GameWorld::onKeyDown(Events::KeyPress& e)
    {
        auto& v = mCharacterControllers[0]->getVelocity();
        switch(e.key())
        {
            case Input::KeyCode::W:
                mCharacterControllers[0]->setVelocity({v.x, v.y, mVelocity});
               break;

            case Input::KeyCode::S:
                mCharacterControllers[0]->setVelocity({v.x, v.y, -mVelocity});
               break;

            case Input::KeyCode::D:
                mCharacterControllers[0]->setVelocity({mVelocity, v.y, v.z});
               break;

            case Input::KeyCode::A:;
                mCharacterControllers[0]->setVelocity({-mVelocity, v.y, v.z});
               break;
            case Input::KeyCode::Space:
               {

               }
               break;
               default:{}
        }
    }

    void W2GameWorld::update()
    {
        if (mCoinCount < mGoal)
        {
        mTimer.begin();
        for (auto& col : mCollisionSystems)
            col->update();

        for (auto& [key, _] : mCylinderColliderComponents)
        {
            mTransformComponents[key].setPosition(
                    mCylinderColliderComponents[key].getPosition().toPoint());
        }

        for (auto& [key, _] : mPlaneColliderComponents)
        {
            auto vv = 
                (mPlaneColliderComponents[key].getPosition() + 
                 (Math::Vector3D){mColliderOffset, mColliderOffset, mColliderOffset}).toPoint();
            mTransformComponents[key].setPosition(vv);
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
                    (mTransformComponents[key].getPosition() - 
                     (Math::Vector3D){mColliderOffset, mColliderOffset, mColliderOffset}).toPoint();
            mPlaneColliderComponents[key].setPosition(vv);
        }


        if (!mCharacterControllers[0]->isGrounded())
        {
            mCharacterControllers[0]->updateVelocity({0, mGravity, 0});
        }

        //Begin Draw System

        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);

        mRenderer->beginDraw();
        mRenderer->setPrimitive(Primitive::Triangle);


        for (auto& [key, value] : mMaterialComponents)
        {
            if (value.isEnabled())
            {
                mRenderer->bindShader(value.getShaderId());
                if (mTransformComponents[key].isEnabled())
                {
                    auto currentTransMat = proj * mShuttle.getMatrix() 
                        * mTransformComponents[key].getMatrix()
                        * Math::Util::scaleMatrix(mTransformComponents[key].getScale()); 
                    mRenderer->updatePushConstant(&currentTransMat, 0);
                }
                auto col = value.getColour();
                mRenderer->updateUniform(&col, 0, key == "p2");
                mRenderer->render();
            }
        }

        mRenderer->endDraw();
        auto followPos = mTransformComponents["p1"].getPosition();
        mShuttle.setPosition(followPos.x, followPos.y - 10, followPos.z - 15 * 4);
        // End Drawing System

        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
        }

    }
    
}}

#include "simplest_world.h"
#include <cstdlib>

namespace Lina { namespace Games {

    void SimplestGameWorld::preinit()
    {   
        GameSystem::GameObject firstPlayer("p1");    
        GameSystem::GameObject planet("planet");

        ECS::CollisionSystem* cSystem = new ECS::CollisionSystem();
        addCollisionSystem(cSystem);

        firstPlayer.addComponent(ComponentType::Transform);
        firstPlayer.addComponent(ComponentType::CylinderCollider);
        firstPlayer.addComponent(ComponentType::Material);


        planet.addComponent(ComponentType::Transform);
        planet.addComponent(ComponentType::MeshCollider);
        planet.addComponent(ComponentType::Material);

        addObject(firstPlayer);
        addObject(planet);

        fetchComponents(); 
        for (auto& [tag, value] : mCylinderColliderComponents)
        {
            value.setCallDefaults(false);
            cSystem->addCollider(&value);
        }
        f32 rad = 1.0f;
        mMeshColliderComponents["planet"].setScale({rad, rad, rad});
        mMeshColliderComponents["planet"].setVertices(mEnvVerts);
        mMeshColliderComponents["planet"].setIndices(mEnvInds);
        mTransformComponents["planet"].setScale({rad, rad, rad});

        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }


        cSystem->staticOptimize();
        //cSystem->constructGrid({-rad, -rad, -rad}, {rad, rad, rad}, 50);
        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();
    }


    void SimplestGameWorld::setCallBacks()
    {
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                std::cerr << "ENTER: (" << c1->getTag() << ", " << c2->getTag() << ")\n";
            }; 

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionPersist = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                std::cerr << "Persist: (" << c1->getTag() << ", " << c2->getTag() << ")\n";
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionExit = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                std::cerr << "Exit: (" << c1->getTag() << ", " << c2->getTag() << ")\n";
            };


        mCylinderColliderComponents["p1"].setOnCollisionEnter(
                playerCollisionEnter);

        mCylinderColliderComponents["p1"].setOnCollisionPersist(
                playerCollisionPersist);
        mCylinderColliderComponents["p1"].setOnCollisionExit(playerCollisionExit);
    }

    void SimplestGameWorld::init()
    {
        mShuttle.enableRotations();
        mShuttle.setSenitivity(2.0f);
        mCylinderColliderComponents["p1"].setPosition({0, 0, 0});
        mCylinderColliderComponents["p1"].setHeight(2);
        mCylinderColliderComponents["p1"].setRadius(1);
        mCylinderColliderComponents["p1"].setCallDefaults(false);
        mCharacterControllers[0]->setVelocity({0, 0, 0});
        mCharacterControllers[0]->setRotationVelocity({0, 0, 0});

        mTransformComponents["p1"].setPosition({0, 0, 0});
        mTransformComponents["p1"].setScale({1, 2, 1});
        mMaterialComponents["p1"].setColour({(float)0x73 / 0xff, 
                (float)0x87 / 0xff, (float)0x57 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["p1"].setShader(0);

        mMaterialComponents["planet"].setColour({(float)0xff / 0xff, 
                (float)0xff / 0xff, (float)0xff / 0xff, (float)0xff / 0xff});
        mMaterialComponents["planet"].setShader(1);
    }

    void SimplestGameWorld::reset()
    {
        init();
    }

    void SimplestGameWorld::onKeyUp(Events::KeyRelease& e)
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
            case Input::KeyCode::Q:
                {
                    yMove++;                
                }
                break;
            case Input::KeyCode::E:
                {
                    yMove--;
                }
                break;
            case Input::KeyCode::R:
                reset();
                break;
            default:{}
        }
    }

    void SimplestGameWorld::onKeyDown(Events::KeyPress& e)
    {
        auto& v = mCharacterControllers[0]->getVelocity();
        mShuttle.onKeyDown(e);
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
            case Input::KeyCode::Q:
                {
                    yMove = -1;
                }
                break;

            case Input::KeyCode::E:
                {
                    yMove = 1;
                }
                break;
            case Input::KeyCode::O:
                {
                    zoom += zoomChange;
                }
                break;
            case Input::KeyCode::P:
                {
                    zoom -= zoomChange;
                }
                break;
            default:{}
        }
    }
    void SimplestGameWorld::move()
    {
       // auto oldv = mCharacterControllers[0]->getVelocity();
        mCharacterControllers[0]->setVelocity((Math::Vector3D)
                {xMove * 1.0f, yMove * 1.0f, zMove * 1.0f} * translationRate);
    }
    void SimplestGameWorld::update()
    {
        mTimer.begin();
        auto proj = Math::Util::projMatrix(
                135, 
                mRenderer->getWidth() / mRenderer->getHeight(), 
                true);
        auto followPos = mTransformComponents["p1"].getPosition();

        mShuttle.setPosition(followPos.x, followPos.y - 4.0f, followPos.z - zoom);

        for (auto& col : mCollisionSystems)
            col->update();

        move();

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

        mRenderer->beginPasses();
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
                        auto col = value.getColour();
                        mRenderer->updatePushConstant(&col, 1);
                        mRenderer->submitUniformUpdates();
                    }
                    mRenderer->render();
            }
        }
        /*
           mRenderer->render();
           */

        //mRenderer->endPass();
        mRenderer->bindShader(2);
        mRenderer->submitUniformUpdates();
        mRenderer->nextPass();
        mRenderer->submitUniformUpdates();
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
        
        std::cerr << "ZOOM: " << zoom << '\n';
        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());
    }

}}

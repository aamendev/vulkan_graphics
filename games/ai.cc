#include "ai.h"
#include <cstdlib>

namespace Lina { namespace Games {

    void AiGameWorld::preinit()
    {   
        currentCollectible = 0;
        GameSystem::GameObject firstPlayer("p1");    
        GameSystem::GameObject secondPlayer("p2");
        GameSystem::GameObject thirdPlayer("p3");
        GameSystem::GameObject token("token");
        GameSystem::GameObject plane("plane");
        GameSystem::GameObject losingFloor("lose");

        ECS::CollisionSystem* cSystem = new ECS::CollisionSystem();
        addCollisionSystem(cSystem);

        firstPlayer.addComponent(ComponentType::Transform);
        firstPlayer.addComponent(ComponentType::Health);
        firstPlayer.addComponent(ComponentType::CylinderCollider);
        firstPlayer.addComponent(ComponentType::Material);

        token.addComponent(ComponentType::Transform);
        token.addComponent(ComponentType::CylinderCollider);
        token.addComponent(ComponentType::Material);

        secondPlayer.addComponent(ComponentType::Transform);
        secondPlayer.addComponent(ComponentType::Health);
        secondPlayer.addComponent(ComponentType::CylinderCollider);
        secondPlayer.addComponent(ComponentType::Material);
        secondPlayer.addComponent(ComponentType::AI);

        thirdPlayer.addComponent(ComponentType::Transform);
        thirdPlayer.addComponent(ComponentType::Health);
        thirdPlayer.addComponent(ComponentType::CylinderCollider);
        thirdPlayer.addComponent(ComponentType::Material);
        thirdPlayer.addComponent(ComponentType::AI);

        plane.addComponent(ComponentType::Transform);
        plane.addComponent(ComponentType::MeshCollider);
        plane.addComponent(ComponentType::Material);

        losingFloor.addComponent(ComponentType::Transform);
        losingFloor.addComponent(ComponentType::PlaneCollider);
        losingFloor.addComponent(ComponentType::Material);
        losingFloor.removeComponent(ComponentType::Material);

        addObject(firstPlayer);
        addObject(secondPlayer);
        addObject(thirdPlayer);
        addObject(plane);
        addObject(losingFloor);
        addObject(token);

        fetchComponents(); 
        for (auto& [tag, value] : mCylinderColliderComponents)
        {
            if (tag != "p2")
            {
                value.setCallDefaults(false);
            }
            cSystem->addCollider(&value);
        }
        f32 rad = 20.0f;
        mMeshColliderComponents["plane"].setScale({rad, rad, rad});
        mMeshColliderComponents["plane"].setRotation({0, 0, PI});
        mMeshColliderComponents["plane"].setVertices(mEnvVerts);
        mMeshColliderComponents["plane"].setIndices(mEnvInds);
        mTransformComponents["plane"].setScale({rad, rad, rad});
        mTransformComponents["plane"].setRotation({0, 0, PI});

        for (auto& [_, value] : mPlaneColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }
        for (auto& [_, value] : mMeshColliderComponents)
        {
            cSystem->addCollider(&value, ColliderType::Static);
        }


        cSystem->optimize();
        cSystem->constructGrid({-200, -200, -200}, {200, 200, 200}, 50);
        ECS::CharacterController* c = new ECS::CharacterController(&mTransformComponents["p1"]);  
        addCharacterController(c);
        setCallBacks();
    }


    void AiGameWorld::setCallBacks()
    {
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                if (c2->getTag() == "plane")
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
                else if (c2->getTag() == "lose")
                {
                    reset();
                }
            };
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            p3CollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                if (c2->getTag() == "plane")
                {
                    auto* d = (ECS::Components::Colliders::Mesh*)c2;
                    auto inf = d->getInfo();
                    mTransformComponents["p2"].translate(inf.normal * inf.depth);
                }
            };
        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            tokenCollisionEnter = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                c1->setCallDefaults(false);
                if (c2->getTag() == "p3")
                {
                    u32 oldColl = currentCollectible;
                    while (oldColl == currentCollectible)
                    {
                    currentCollectible = rand() % (mAiGraphs[0].getSize());
                    }

                    auto currNode = mAiGraphs[0][currentCollectible].first;
                    auto scale = mTransformComponents["plane"].getScale();
                    auto rot = mTransformComponents["plane"].getRotation();
                    auto meshPos = mTransformComponents["plane"].getPosition();
                    auto trans = rot.getRotationMatrix4D() * Math::Util::scaleMatrix(scale);
                    auto finalPos = trans * Math::Point3D(currNode->x, currNode->y, currNode->z)
                        + meshPos;
                    mTransformComponents["token"].setPosition(
                            finalPos                
                            );
                    mCylinderColliderComponents["token"].setPosition(
                            finalPos                
                            );
                }
            };
        /*std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
          tokenCollisionPersist = 
          [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
          mutable{
          c1->setCallDefaults(false);
          if (c2->getTag() == "p3")
          {
          auto currNode = mAiGraphs[0][currentCollectible].first;
          currentCollectible = rand() % (mAiGraphs[0].getSize());
          mTransformComponents["p3"].setPosition(
          {currNode->x,
          currNode->y,
          currNode->z}
          );
          }
          };*/

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionPersist = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{

                if (c2->getTag() == "plane")
                {
                    static u32 collisionCount = 0;
                    auto* d = (ECS::Components::Colliders::Mesh*)c2;
                    auto inf = d->getInfo();
                    {
                        mTransformComponents[c1->getTag()].translate(inf.normal * 
                                (inf.depth));
                    }
                }
            };

        std::function<void(ECS::Components::Collider*, ECS::Components::Collider*)>
            playerCollisionExit = 
            [this](ECS::Components::Collider* c1, ECS::Components::Collider* c2) 
            mutable{
                auto* d1 = dynamic_cast<ECS::Components::Colliders::Cylinder*>(c1);
                if(c2->getTag() == "plane") 
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
        mCylinderColliderComponents["token"].setOnCollisionEnter(tokenCollisionEnter);
        mCylinderColliderComponents["p3"].setOnCollisionEnter(p3CollisionEnter);


        using namespace Helpers::AI;
        class Follow : public AIBehaviour
        {
            public:
                Follow(ECS::Components::Transform* aiTrans,
                        ECS::Components::Transform* followTrans,
                        f32 maxSpeed = 1.0f):
                    mAiTrans(aiTrans), mFollowTrans(followTrans),
                    mMaxSpeed(maxSpeed){}
                Helpers::AI::BehaviourStatus run() override 
                {
                    auto dir =
                        (mFollowTrans->getPosition() - mAiTrans->getPosition())
                        .normalise();  
                    mAiTrans->translate(dir * mMaxSpeed);

                    return BehaviourStatus::Success;
                }
            private:
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                f32 mMaxSpeed;
        };

        class OrientAndFollow : public AIBehaviour
        {
            public:
                OrientAndFollow(ECS::Components::Transform* aiTrans,
                        ECS::Components::Transform* followTrans,
                        f32 maxSpeed = 1.0f):
                    mAiTrans(aiTrans), mFollowTrans(followTrans),
                    mMaxSpeed(maxSpeed){}
                Helpers::AI::BehaviourStatus run() override 
                {
                    auto dir =
                        (mFollowTrans->getPosition() - mAiTrans->getPosition())
                        .normalise();  
                    auto xzProj = dir.planeProject(Math::Util::yAxis()).normalise();
                    auto xAngle = atan2f(xzProj.z, xzProj.x);
                    auto yAngle = acosf(dir.dot(Math::Util::yAxis()));
                    auto zAngle = dir.dot(Math::Util::zAxis());
                    Math::EulerAngles targetOrientation = {xAngle, yAngle, zAngle};
                    auto currentOrientiation = 
                        mAiTrans->getRotationAngles();
                    auto currY = (currentOrientiation.x == 0) * 
                        (currentOrientiation.y + PI/2)
                        + !(currentOrientiation.x ==0) * (-currentOrientiation.y - PI/2);
                    auto diff = currY - targetOrientation.x;
                    if (abs(diff) <= 0.2f)
                    {
                        mAiTrans->translate(dir * mMaxSpeed);
                        if (abs(diff) >= 0.05f)
                        {
                            mAiTrans->rotate({0.0f, 
                                    -diff / 2
                                    , 0.0f});
                        }
                    }
                    else 
                    {
                        mAiTrans->rotate({0.0f, 0.10f, 0.0f});
                    }
                    return BehaviourStatus::Success;
                }
            private:
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                f32 mMaxSpeed;
        };

        class SmartPursue : public AIBehaviour
        {
            public:
                SmartPursue(
                        ECS::Components::Transform* aiTrans,
                        ECS::Components::Transform* followTrans,
                        ECS::CharacterController* charController,
                        f32 maxSpeed = 1.0f):
                    mAiTrans(aiTrans), 
                    mFollowTrans(followTrans),
                    mCharController(charController),
                    mMaxSpeed(maxSpeed){}
                Helpers::AI::BehaviourStatus run() override 
                {
                    auto dir1 =
                        (mFollowTrans->getPosition() - mAiTrans->getPosition());  
                    auto dir = (mCharController->getVelocity() + dir1).normalise();
                    mAiTrans->translate(dir * mMaxSpeed);
                    return BehaviourStatus::Success;
                }
            private:
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                ECS::CharacterController* mCharController;
                f32 mMaxSpeed;
        };


        class Sneak : public AIBehaviour
        {
            public:
                Sneak(
                        ECS::Components::Transform* aiTrans,
                        ECS::Components::Transform* followTrans,
                        f32 coneAngle = PI/2):
                    mAiTrans(aiTrans), 
                    mFollowTrans(followTrans),
                    mConeAngle(coneAngle){}

                Helpers::AI::BehaviourStatus run() override
                {
                    auto dir =
                        (mAiTrans->getPosition() - mFollowTrans->getPosition())
                        .normalise();  
                    auto xzProj = dir.planeProject(Math::Util::yAxis()).normalise();
                    auto xAngle = atan2f(xzProj.z, xzProj.x);
                    auto yAngle = acosf(dir.dot(Math::Util::yAxis()));
                    auto zAngle = dir.dot(Math::Util::zAxis());
                    Math::EulerAngles targetOrientation = {xAngle, yAngle, zAngle};
                    auto currentOrientiation = 
                        mFollowTrans->getRotationAngles();
                    auto currY = (currentOrientiation.x == 0) * 
                        (currentOrientiation.y + PI/2)
                        + !(currentOrientiation.x ==0) * (-currentOrientiation.y - PI/2);
                    auto diff = currY - targetOrientation.x;
                    if (abs(diff) > mConeAngle)
                    {
                        return BehaviourStatus::Success;
                    }
                    else {
                        return BehaviourStatus::Fail;
                    }
                }
            private:
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                f32 mConeAngle;
        };
        class SafeDistance : public AIBehaviour
        {
            public:
                SafeDistance(
                        ECS::Components::Transform* aiTrans,
                        ECS::Components::Transform* followTrans,
                        f32 minDistance = 100.0f):
                    mAiTrans(aiTrans), 
                    mFollowTrans(followTrans),
                    mMinDistance(minDistance){}
                Helpers::AI::BehaviourStatus run() override 
                {
                    auto distanceSquared=
                        (mFollowTrans->getPosition() - mAiTrans->getPosition()).squaredMagnitude();  
                    if (distanceSquared > mMinDistance * mMinDistance)
                    {
                        return BehaviourStatus::Success;
                    }
                    else {
                        return  BehaviourStatus::Fail;
                    }
                }
            private:
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                f32 mMinDistance;
        };

        class TraverseGraph : public AIBehaviour
        {
            public:
                TraverseGraph(
                        ECS::Components::Transform* ai, 
                        ECS::Components::Transform* follow,
                        ECS::Components::Transform* navMesh,
                        Graph* g, u32 start, u32 end, u32* update): 
                    mGraph(g),
                    mStart(start), mEnd(end), 
                    finished(true), mPath({}), 
                    mCurrent(0),
                    mAiTrans(ai), mFollowTrans(follow),
                    mMaxSpeed(1.0f), mNext(update), mNavMesh(navMesh){}

                inline void setEnd(u32 end) {mEnd = end;}
                inline void setStart(u32 start) {mStart = start;}
                Helpers::AI::BehaviourStatus run() override 
                {
                    if (finished)
                    {
                        mEnd = *mNext;
                        mGraph->setStart(mStart);
                        mGraph->setEnd(mEnd);
                        mGraph->traverse();
                        mPath = mGraph->getPath();
                        finished = false;
                    }
                    else [[likely]]
                    {
                        if (mPath[mCurrent] == mEnd)
                        {
                            auto dir =
                                (mFollowTrans->getPosition() - mAiTrans->getPosition())
                                .normalise();  
                            auto myPos = mFollowTrans->getPosition();
                            auto pos = mAiTrans->getPosition();
                            auto mag = (myPos - pos).magnitude();
                            mAiTrans->translate(dir * fmin(mag, mMaxSpeed));
                            /*if (fabs(myPos.x - pos.x) < 0.01f && 
                                    fabs(myPos.z - pos.z) < 0.01f)
                                {
                                    mCurrent++;
                                }*/
                            if (*mNext != mEnd)
                            {
                            finished = true;
                            mStart = mEnd;
                            mCurrent = 0;
                            return BehaviourStatus::Success;
                            }
                            return BehaviourStatus::Pending;
                        }
                        else [[likely]]
                        {
                            auto scale = mNavMesh->getScale();
                            auto rot = mNavMesh->getRotation();
                            auto meshPos = mNavMesh->getPosition();
                            auto trans = 
                                rot.getRotationMatrix4D() * Math::Util::scaleMatrix(scale);
                            auto* node = mGraph->at(mPath[mCurrent]).first;
                            Math::Vector3D pos = {node->x, node->y, node->z};
                            pos = trans * pos + meshPos;
                            auto dir =
                                (pos - mAiTrans->getPosition()).normalise();
                            auto mag = (pos - mAiTrans->getPosition()).magnitude();
                            mAiTrans->translate(dir * fmin(mag, mMaxSpeed));
                            auto myPos = mAiTrans->getPosition();
                            if (fabs(myPos.x - pos.x) < 0.01f && 
                                    fabs(myPos.z - pos.z) < 0.01f)
                                {
                                    mCurrent++;
                                }
                            return BehaviourStatus::Pending;
                        }
                    }
                    return BehaviourStatus::Pending;
                }
            private:
                Helpers::AI::Graph* mGraph;
                u32 mStart;
                u32 mEnd;
                u32 mCurrent;
                u32* mNext;
                std::vector<u32> mPath;
                b8 finished;
                f32 mMaxSpeed;
                ECS::Components::Transform* mAiTrans;
                ECS::Components::Transform* mFollowTrans;
                ECS::Components::Transform* mNavMesh;
        };
        auto* p2SmartPuruse = new SmartPursue(
                &mTransformComponents["p2"], 
                &mTransformComponents["p1"],
                mCharacterControllers[0],
                0.5f);
        auto* p2Orient = new OrientAndFollow(
                &mTransformComponents["p2"], 
                &mTransformComponents["p1"],
                1.1f);

        auto* traverse = new TraverseGraph(&mTransformComponents["p3"],
                &mTransformComponents["token"], 
                &mTransformComponents["plane"],
                &mAiGraphs[0], 0, 
                currentCollectible, &currentCollectible);

        auto* p2Sneak = new Sneak(&mTransformComponents["p2"],
                &mTransformComponents["p1"], PI/3);
        auto* p2Safe = new SafeDistance(&mTransformComponents["p2"], 
                &mTransformComponents["p1"], 100.0f);
        LinearSequence* carefulSequence = new LinearSequence();
        LinearSequence* chaseSequence = new LinearSequence();
        LinearSelector* selector = new LinearSelector;
        selector->addChild(chaseSequence);
        selector->addChild(carefulSequence);

        carefulSequence->addChild(p2Sneak);
        carefulSequence->addChild(p2SmartPuruse);

        chaseSequence->addChild(p2Safe);
        chaseSequence->addChild(p2Orient);
        mAIComponents["p2"].setRootBehaviour(selector);
        mAIComponents["p3"].setRootBehaviour(traverse);
    }



    void AiGameWorld::init()
    {
        mCylinderColliderComponents["p1"].setPosition({0, -50, 0});
        mCylinderColliderComponents["p1"].setHeight(20);
        mCylinderColliderComponents["p1"].setRadius(10);
        mCylinderColliderComponents["p1"].setCallDefaults(false);
        mCharacterControllers[0]->setVelocity({0, 0, 0});
        mCharacterControllers[0]->setRotationVelocity({0, 0, 0});

        mTransformComponents["p1"].setPosition({0, -50, 0});
        mTransformComponents["p1"].setScale({5, 20, 10});

        mMaterialComponents["p1"].setColour({(float)0x73 / 0xff, 
                (float)0x87 / 0xff, (float)0x57 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["p1"].setShader(0);

        mTransformComponents["p2"].setScale({5, 20, 10});
        mMaterialComponents["p2"].setColour(
                {(float)0xaa / 0xff,
                (float)0xff / 0xff,
                (float)0xff / 0xff,
                (float)0xff / 0xff}
                );

        mMaterialComponents["p2"].setShader(0);
        mCylinderColliderComponents["p2"].setHeight(20);
        mCylinderColliderComponents["p2"].setRadius(10);
        mTransformComponents["p2"].setPosition({0, -10, 0});
        mCylinderColliderComponents["p2"].setPosition({0, 0 ,0});

        mTransformComponents["p3"].setScale({5, 20, 10});
        mMaterialComponents["p3"].setColour(
                {(float)0xff / 0xff,
                (float)0x44 / 0xff,
                (float)0x22 / 0xff,
                (float)0xff / 0xff}
                );

        mMaterialComponents["p3"].setShader(0);
        mCylinderColliderComponents["p3"].setHeight(20);
        mCylinderColliderComponents["p3"].setRadius(20);
        mTransformComponents["p3"].setPosition({0, -8, 0});
        mCylinderColliderComponents["p3"].setPosition({0, 0 ,0});

        auto rad = 5.0f;
        mTransformComponents["token"].setScale({rad, rad, rad});
        mMaterialComponents["token"].setColour(
                {(float)0x78 / 0xff,
                (float)0xa6 / 0xff,
                (float)0x58 / 0xff,
                (float)0xff / 0xff}
                );

        mMaterialComponents["token"].setShader(0);
        mCylinderColliderComponents["token"].setHeight(rad);
        mCylinderColliderComponents["token"].setRadius(rad);
        currentCollectible = 0;
        auto currNode = mAiGraphs[0][currentCollectible].first;
        auto scale = mTransformComponents["plane"].getScale();
        auto rot = mTransformComponents["plane"].getRotation();
        auto meshPos = mTransformComponents["plane"].getPosition();
        auto trans = rot.getRotationMatrix4D() * Math::Util::scaleMatrix(scale);
        auto finalPos = trans * Math::Point3D(currNode->x, currNode->y, currNode->z)
            + meshPos;
        mTransformComponents["token"].setPosition(finalPos);
        mCylinderColliderComponents["token"].setPosition(finalPos);

        mMaterialComponents["plane"].setColour({(float)0x6B / 0xff, 
                (float)0x57 / 0xff, (float)0x87 / 0xff, (float)0xff / 0xff});
        mMaterialComponents["plane"].setShader(1);

        mTransformComponents["lose"].disable();
        mPlaneColliderComponents["lose"].setLength(2000);
        mPlaneColliderComponents["lose"].setWidth(2000);
        mPlaneColliderComponents["lose"].setPosition({0, 200, 0});
    }

    void AiGameWorld::reset()
    {
        init();
    }

    void AiGameWorld::onKeyUp(Events::KeyRelease& e)
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

    void AiGameWorld::onKeyDown(Events::KeyPress& e)
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
    void AiGameWorld::move()
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
    void AiGameWorld::update()
    {
        mTimer.begin();
        for (auto& col : mCollisionSystems)
            col->update();

        move();

        for (auto& [key, _] : mCylinderColliderComponents)
        {
            if (key != "p1")
            {
            mTransformComponents[key].setPosition(
                    mCylinderColliderComponents[key].getPosition().toPoint());
            }
        }

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

        for (auto& [_, value] : mAIComponents)
        {
            value.update();
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
                mRenderer->updateUniform(&col, 0, 
                        (key == "plane")
                        + 2 * (key == "p2")
                        + 3 * (key == "p3")
                        + 4 * (key == "token"));
                mRenderer->render();
            }
        }
        /*
        mRenderer->bindShader(2);
        struct Time 
        {
            f32 w;
            f32 h;
            f32 t;
        };
        Time currTime;
        currTime.h = mRenderer->getHeight();
        currTime.w = mRenderer->getWidth();
        currTime.t = ticks;
        ticks += 0.001f;
        mRenderer->updatePushConstant(&currTime, 0);
        mRenderer->render();
        */

        mRenderer->endDraw();
        auto followPos = mTransformComponents["p1"].getPosition();
        mShuttle.setPosition(followPos.x, followPos.y - 3, followPos.z - 4*15);
        // End Drawing System

        mTimer.end();

        mTimer.wait((1000/mFrameRate) - mTimer.getTime());

    }

}}

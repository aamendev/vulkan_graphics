#include "collision_system.h"


namespace Lina { namespace ECS {

    void CollisionSystem::update()
    {
        for (int i = 0; i < mColliders.size() - 1; i++)
        {
            for (int j = i + 1; j < mColliders.size(); j++)
            {
                auto& tagi = mColliders[i]->getTag();
                auto& tagj = mColliders[j]->getTag();
                if (mColliders[i]->checkCollision(mColliders[j]) &&
                        mColliders[i]->isEnabled() && mColliders[j]->isEnabled())
                {
                    switch(mColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Enter);
                                mColliders[i]->onCollisionEnter(mColliders[j]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Persist);

                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mColliders[i]->onCollisionPersist(mColliders[j]);
                            }
                            break;
                    }
                    
                    switch(mColliders[j]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Enter);
                                mColliders[j]->onCollisionEnter(mColliders[i]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Persist);
                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mColliders[j]->onCollisionPersist(mColliders[i]);
                            }
                            break;
                    }
                }
                else 
                {
                    switch(mColliders[i]->getStatus(mColliders[j]->getTag()))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mColliders[i]->removeStatus(tagj);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Exit);
                                mColliders[i]->onCollisionExit(mColliders[j]);
                            }
                            break;
                    }

                    switch(mColliders[j]->getStatus(mColliders[i]->getTag()))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mColliders[j]->removeStatus(tagi);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Exit);
                                mColliders[j]->onCollisionExit(mColliders[i]);
                            }
                            break;
                    }
                }
                
            }
        }
    }

    void CollisionSystem::enable()
    {
        mEnabled = true;
    }

    void CollisionSystem::disable()
    {
        mEnabled = false;
    }
}}

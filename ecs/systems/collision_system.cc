#include "collision_system.h"
#include <algorithm>


namespace Lina { namespace ECS {

    void CollisionSystem::addCollider(Components::Collider* c,
            ColliderType t) 
    {
        switch(t)
        {
            case ColliderType::Static:
                {
                    mStaticColliders.push_back(c);
                    mStaticRegistriy[c->getTag()] = mStaticColliders.size() - 1;
                }
                break;
            case ColliderType::Dynamic:
                {
                    mDynamicColliders.push_back(c);
                }
        }
    }
    
    void CollisionSystem::update()
    {
        switch(mOptMode)
        {
            case CollisionOptimization::Naive:
                {
                    naiveUpdate();
                }
                break;
            case CollisionOptimization::Optimized:
                {
                    optimizedUpdate();
                }
                break;
        }
    }

    void CollisionSystem::naiveUpdate()
    {
        // Dynamic Collider Checks
        for (int i = 0; i < mDynamicColliders.size(); i++)
        {
            for (int j = i + 1; j < mDynamicColliders.size(); j++)
            {
                auto& tagi = mDynamicColliders[i]->getTag();
                auto& tagj = mDynamicColliders[j]->getTag();
                if (mDynamicColliders[i]->checkCollision(mDynamicColliders[j]) &&
                        mDynamicColliders[i]->isEnabled() && mDynamicColliders[j]->isEnabled())
                {
                    switch(mDynamicColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Enter);
                                mDynamicColliders[i]->onCollisionEnter(mDynamicColliders[j]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Persist);

                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->onCollisionPersist(mDynamicColliders[j]);
                            }
                            break;
                    }

                    switch(mDynamicColliders[j]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mDynamicColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Enter);
                                mDynamicColliders[j]->onCollisionEnter(mDynamicColliders[i]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mDynamicColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Persist);
                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[j]->onCollisionPersist(mDynamicColliders[i]);
                            }
                            break;
                    }
                }
                else 
                {
                    switch(mDynamicColliders[i]->getStatus(mDynamicColliders[j]->getTag()))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mDynamicColliders[i]->removeStatus(tagj);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Exit);
                                mDynamicColliders[i]->onCollisionExit(mDynamicColliders[j]);
                            }
                            break;
                    }

                    switch(mDynamicColliders[j]->getStatus(mDynamicColliders[i]->getTag()))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mDynamicColliders[j]->removeStatus(tagi);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Exit);
                                mDynamicColliders[j]->onCollisionExit(mDynamicColliders[i]);
                            }
                            break;
                    }
                }
            }
        }
        // Dynamic with Static Colliders check
        
        for (int i = 0; i < mDynamicColliders.size(); i++)
        {
            for (int j = 0; j < mStaticColliders.size(); j++)
            {
                auto& tagi = mDynamicColliders[i]->getTag();
                auto& tagj = mStaticColliders[j]->getTag();
                if (mDynamicColliders[i]->checkCollision(mStaticColliders[j]) &&
                        mDynamicColliders[i]->isEnabled() && mStaticColliders[j]->isEnabled())
                {
                    switch(mDynamicColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Enter);
                                mDynamicColliders[i]->onCollisionEnter(mStaticColliders[j]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Persist);

                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->onCollisionPersist(mStaticColliders[j]);
                            }
                            break;
                    }

                    switch(mStaticColliders[j]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mStaticColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Enter);
                                mStaticColliders[j]->onCollisionEnter(mDynamicColliders[i]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mStaticColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Persist);
                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mStaticColliders[j]->onCollisionPersist(mDynamicColliders[i]);
                            }
                            break;
                    }
                }
                else 
                {
                    switch(mDynamicColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mDynamicColliders[i]->removeStatus(tagj);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Exit);
                                mDynamicColliders[i]->onCollisionExit(mStaticColliders[j]);
                            }
                            break;
                    }

                    switch(mStaticColliders[j]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mStaticColliders[j]->removeStatus(tagi);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mStaticColliders[j]->setStatus(
                                        tagi, Components::CollisionStatus::Exit);
                                mStaticColliders[j]->onCollisionExit(mDynamicColliders[i]);
                            }
                            break;
                    }
                }
            }
        }
    
    }

    void CollisionSystem::optimizedUpdate()
    {
        // Dynamic with Dynamic: Grid

        // Dynamic with Static: Bvh
        for (int i = 0; i < mDynamicColliders.size(); i++)
        {
            auto tagi = mDynamicColliders[i]->getTag();
            auto [tagsj, ntagsj] = checkCollideBvh(mDynamicColliders[i], &mFullBvh);
            for (const auto& tagj : tagsj)
            {
                auto collidedIdx = mStaticRegistriy[tagj];
                switch(mDynamicColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Enter);
                                mDynamicColliders[i]->onCollisionEnter(
                                        mStaticColliders[collidedIdx]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Persist);

                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->onCollisionPersist(
                                        mStaticColliders[collidedIdx]);
                            }
                            break;
                    }

                    switch(mStaticColliders[collidedIdx]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {
                                mStaticColliders[collidedIdx]->setStatus(
                                        tagi, Components::CollisionStatus::Enter);
                                mStaticColliders[collidedIdx]->onCollisionEnter(
                                        mDynamicColliders[i]);
                            }
                            break;
                        case Components::CollisionStatus::Enter:
                            {
                                mStaticColliders[collidedIdx]->setStatus(
                                        tagi, Components::CollisionStatus::Persist);
                            }
                        case Components::CollisionStatus::Persist:
                            {
                                mStaticColliders[collidedIdx]->onCollisionPersist(mDynamicColliders[i]);
                            }
                            break;
                    }
                }
                for (const auto& tagj : ntagsj) 
                {

                    auto collidedIdx = mStaticRegistriy[tagj];
                    switch(mDynamicColliders[i]->getStatus(tagj))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mDynamicColliders[i]->removeStatus(tagj);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mDynamicColliders[i]->setStatus(
                                        tagj, Components::CollisionStatus::Exit);
                                mDynamicColliders[i]->onCollisionExit(
                                        mStaticColliders[collidedIdx]);
                            }
                            break;
                    }

                    switch(mStaticColliders[collidedIdx]->getStatus(tagi))
                    {
                        case Components::CollisionStatus::None:
                        case Components::CollisionStatus::Exit:
                            {mStaticColliders[collidedIdx]->removeStatus(tagi);}
                            break;
                        case Components::CollisionStatus::Enter:
                        case Components::CollisionStatus::Persist:
                            {
                                mStaticColliders[collidedIdx]->setStatus(
                                        tagi, Components::CollisionStatus::Exit);
                                mStaticColliders[collidedIdx]->onCollisionExit(mDynamicColliders[i]);
                            }
                            break;
                    }
                }

        }
    }

    std::pair<std::set<std::string>, std::set<std::string>>
        CollisionSystem::checkCollideBvh(
                Components::Collider* c,
                Components::Collider::BVH* b)
    {
        if (b == nullptr)
        {
            return {{}, {}};
        }
        std::set<std::string> collidedTags;
        std::set<std::string> nCollidedTags;

        if (c->checkCollision(b->root))
        {
            if (b->left == nullptr && b->right == nullptr)
            {
                collidedTags.insert(b->root->getTag());
            }
            else 
            {
                auto [leftCollided, z] = checkCollideBvh(c, b->left);
                (void)z;
                collidedTags.merge(leftCollided);
                auto [rightCollided, y] = checkCollideBvh(c, b->right);
                (void)y;
                collidedTags.merge(rightCollided);
            }
        }
        else 
        {
           auto nTag = b->root->getTag(); 
           auto iter = std::find(nTag.cbegin(), nTag.cend(), ';');
           auto begin = nTag.cbegin();
           while (iter != nTag.cend())
           {
               nCollidedTags.emplace(begin, iter);
               begin = iter + 1;
               iter = std::find(begin, nTag.cend(), ';');
           }
        }

        return {collidedTags, nCollidedTags};
    }

    void CollisionSystem::combineBVHs()
    {
        std::vector<Components::Collider::BVH*> mbvhs;
        mbvhs.reserve(mStaticColliders.size());
        for (int i = 0; i < mStaticColliders.size(); i++)
        {
            mStaticColliders[i]->computeBVH();
           mbvhs.push_back(mStaticColliders[i]->getBVHPointer()); 
        }

        int n = mbvhs.size();
        std::cerr << "BVH Count: " << n << '\n';
        int power = 1;
        b8 reverse = false; // to balance tree
        while (n > power)
        {
            if (!reverse)
            {
                for (int i = 0; i < n - power; i += power<<1)
                {
                    combineBVH(mbvhs[i], mbvhs[i + power]);
                }
            }
            else if (reverse)
            {
                for (int i = ((n-1) / power) * power; i >= power; i -= power<<1)
                {
                    combineBVH(mbvhs[i -power ], mbvhs[i]);
                }
            }
            reverse = !reverse;
            power <<= 1;
        }
        mFullBvh = *mbvhs[0];
    }
    void CollisionSystem::combineBVH(Components::Collider::BVH* b1, 
            Components::Collider::BVH* b2)
    {

        if (b2 == nullptr) return;
        if (b1 == nullptr)
        {
            std::cerr << "Fatal Error, b1 is nullptr\n";
            exit(1);
        }
        Components::Collider::BVH* newLeft = new Components::Collider::BVH;
        auto leftBoundingBox = b1->root->getBoundingBox();     
        auto rightBoundingBox = b2->root->getBoundingBox();     

        std::pair<Math::Point3D, Math::Point3D> combinedBound = 
            std::make_pair(Math::Point3D(
                        std::fmin(leftBoundingBox.first.x, rightBoundingBox.first.x),
                        std::fmin(leftBoundingBox.first.y, rightBoundingBox.first.y),
                        std::fmin(leftBoundingBox.first.z, rightBoundingBox.first.z)
                        ), Math::Point3D(
                        std::fmax(leftBoundingBox.second.x, rightBoundingBox.second.x),
                        std::fmax(leftBoundingBox.second.y, rightBoundingBox.second.y),
                        std::fmax(leftBoundingBox.second.z, rightBoundingBox.second.z)
                            ));
        auto combinedTag = b1->root->getTag() + ";" + b2->root->getTag();
        Components::Colliders::Box* boundCollider = new 
            Components::Colliders::Box(combinedTag, combinedBound);
        newLeft->root = b1->root;
        newLeft->left = b1->left;
        newLeft->right =b1->right;
        b1->left = newLeft;
        b1->right = b2;
        b1->root = boundCollider;
    }
    void CollisionSystem::enable()
    {
        mEnabled = true;
    }

    void CollisionSystem::disable()
    {
        mEnabled = false;
    }

    void CollisionSystem::optimize() 
    {
        mOptMode = CollisionOptimization::Optimized;
        if (!mBvhInit)
        {
            combineBVHs();
            mBvhInit = true;
        }
    }
}}

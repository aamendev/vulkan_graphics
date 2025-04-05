#include "collision_system.h"
#include <algorithm>
#include <iterator>


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
                    mDynamicRegistry[c->getTag()] = mDynamicColliders.size() - 1;
                 //   mDynamicGrid[c->getTag()] = {};
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
                        case Components::CollisionStatus::Exit: { mDynamicColliders[i]->setStatus(
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
        for (int c = 0; c < mGrid.size(); c++)
        {
            updateGrid(c);
            std::vector<std::string> v(mGrid[c].cbegin(), mGrid[c].cend());
            for (int ntagi = 0; !v.empty() && ntagi < v.size() - 1; ntagi++)
            {
                for (int ntagj = ntagi + 1; ntagj < v.size(); ntagj++)
                {
                    auto tagi = v[ntagi];
                    auto tagj = v[ntagj];
                    auto i = mDynamicRegistry[v[ntagi]];
                    auto j = mDynamicRegistry[v[ntagj]];
                    if (mDynamicColliders[i]->checkCollision(mDynamicColliders[j]) &&
                            mDynamicColliders[i]->isEnabled() && mDynamicColliders[j]->isEnabled())
                    {
                        if (tagi == "p1" || tagj == "p1")
                        {
                        std::cerr << tagi << ", " << tagj << "Collided!!\n";
                        }
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
        }

        // Dynamic with Static: Bvh
        static int k =0;
        //for (int i = 0; i < mDynamicColliders.size(); i++)
        for (int i = 0; i < 1; i++)
        {
            auto tagi = mDynamicColliders[i]->getTag();
            std::set<std::string> tagsj;
            std::set<std::string> temp;
            std::vector<std::string> ntagsj;
            checkCollideBvh(mDynamicColliders[i], &mFullBvh,
                    tagsj, temp);

            ntagsj.reserve(tagsj.size() +  temp.size());
            std::set_difference(temp.begin(), temp.end(), 
                    tagsj.begin(), tagsj.end(), std::back_inserter(ntagsj));

            for (const auto& tagj : tagsj)
            {
                k++;
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
                k++;
                if (tagj == "temp") continue;

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

    void CollisionSystem::checkCollideBvh(
            Components::Collider* c,
            Components::Collider::BVH* b, 
            std::set<std::string>& collidedTags,
            std::set<std::string>& nCollidedTags)
    {
        static int counter = 0;
        if (b == nullptr)
        {
            return;
        }
        if (!c->checkCollision(b->root))
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
            nCollidedTags.emplace(begin, iter);
            return;
        }
        if (b->left == nullptr && b->right == nullptr)
        {
            if (b->root->getColliderGeometry() == ColliderGeometry::Mesh)
            {
                auto colTag = b->root->getTag();
                auto* meshCastRoot = (Components::Colliders::Mesh*)(b->root);
                auto* meshCast = (Components::Colliders::Mesh*)
                    (mStaticColliders[mStaticRegistriy[colTag]]);
                meshCast->setCollisionInfo((meshCastRoot->getInfo()));
            }
            collidedTags.insert(b->root->getTag());
        }
        else 
        {
            checkCollideBvh(c, b->left, collidedTags, nCollidedTags);
            checkCollideBvh(c, b->right, collidedTags, nCollidedTags);
        }
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
        int power = 1;
        b8 reverse = false; // to balance tree
        int i;
        int final = 0;
        while (n > power)
        {
            if (!reverse)
            {
                for (i = 0; i < n; i += power<<1)
                {
                    if (i < n-power)
                    {
                        combineBVH(mbvhs[i], mbvhs[i + power]);
                        final = i + 1;
                    }
                    else 
                    {
                        combineBVH(mbvhs[0], mbvhs[i]);
                        final = (i - (power<<1)) + 1;
                    }
                }
                n = final;
            }
            else if (reverse)
            {
                for (int i = ((n-1) / power) * power; i >= power; i -= power<<1)
                {
                    combineBVH(mbvhs[i - power ], mbvhs[i]);
                }
            }
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
        boundCollider->computeBoundingBox();
        newLeft->root = b1->root;
        newLeft->left = b1->left;
        newLeft->right =b1->right;
        b1->left = newLeft;
        b1->right = b2;
        b1->root = boundCollider;
    }

    void CollisionSystem::updateGrid(u32 i)
    {
        std::vector<std::string> vTags(mGrid[i].begin(), mGrid[i].end());
        for (auto tag : vTags)
        {
            auto colId = mDynamicRegistry[tag];
            if (!mDynamicColliders[colId]->checkMoved()) continue;
            mDynamicColliders[colId]->cohere();
            auto [min, max] = mGridPos;
            auto h = mGridSize;
            mDynamicColliders[colId]->computeBoundingBox();
            auto& box = mDynamicColliders[colId]->getBoundingBox();
            u32 xCount = (max.x - min.x) / h;
            u32 yCount = (max.y - min.y) / h;
            u32 zCount = (max.z - min.z) / h;
               u32 fxIdx = (box.first.x - min.x) / h;
               u32 sxIdx = (box.second.x - min.x) / h;
               u32 fyIdx = (box.first.y - min.y) / h;
               u32 syIdx = (box.second.y - min.y) / h;
               u32 fzIdx = (box.first.z - min.z) / h;
               u32 szIdx = (box.second.z - min.z) / h;
            std::array<u32, 8> possibleInds = 
            {
                (u32)(fxIdx + fyIdx * xCount +fzIdx * xCount * yCount),
                (u32)(sxIdx + fyIdx * xCount +fzIdx * xCount * yCount),
                (u32)(fxIdx + syIdx * xCount +fzIdx * xCount * yCount),
                (u32)(sxIdx + syIdx * xCount +fzIdx * xCount * yCount),

                (u32)(fxIdx + fyIdx * xCount +szIdx * xCount * yCount),
                (u32)(sxIdx + fyIdx * xCount +szIdx * xCount * yCount),
                (u32)(fxIdx + syIdx * xCount +szIdx * xCount * yCount),
                (u32)(sxIdx + syIdx * xCount +szIdx * xCount * yCount),
            };

                if (std::find(possibleInds.cbegin(), possibleInds.cend(), i)
                        != possibleInds.end()) continue;
                mGrid[i].erase(tag);
               

                
                mGrid[fxIdx + fyIdx * xCount + fzIdx * xCount * yCount].insert(tag);
                

                mGrid[sxIdx + fyIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + syIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + syIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + fyIdx * xCount +szIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + fyIdx * xCount +szIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + syIdx * xCount +szIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + syIdx * xCount +szIdx * xCount * yCount].insert(tag);
                
                /*
                mDynamicGrid[tag].insert(fxIdx + fyIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + fyIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + syIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + syIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + fyIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + fyIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + syIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + syIdx * xCount +szIdx * xCount * yCount);
                */
        }
    }
    void CollisionSystem::constructGrid(Math::Point3D min, 
                    Math::Point3D max, u32 h)
    {
        mGridPos = std::make_pair(min, max);  
        mGridSize = h;
        u32 xCount = (max.x - min.x) / h;
        u32 yCount = (max.y - min.z) / h;
        u32 zCount = (max.z - min.z) / h;
        mGrid.resize(xCount * yCount * zCount);
        for (int i = 0; i < mDynamicColliders.size(); i++)
        {
            mDynamicColliders[i]->cohere();
            auto tag = mDynamicColliders[i]->getTag();
               mDynamicColliders[i]->computeBoundingBox(); 
               auto& box = mDynamicColliders[i]->getBoundingBox();
               u32 fxIdx = (box.first.x - min.x) / h;
               u32 sxIdx = (box.second.x - min.x) / h;
               u32 fyIdx = (box.first.y - min.y) / h;
               u32 syIdx = (box.second.y - min.y) / h;
               u32 fzIdx = (box.first.z - min.z) / h;
               u32 szIdx = (box.second.z - min.z) / h;
               // object can be in 8 possible grid cubes;
                mGrid[fxIdx + fyIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + fyIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + syIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + syIdx * xCount +fzIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + fyIdx * xCount +szIdx].insert(tag);

                mGrid[sxIdx + fyIdx * xCount +szIdx * xCount * yCount].insert(tag);

                mGrid[fxIdx + syIdx * xCount +szIdx * xCount * yCount].insert(tag);

                mGrid[sxIdx + syIdx * xCount +szIdx * xCount * yCount].insert(tag);
/*
                mDynamicGrid[tag].insert(fxIdx + fyIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + fyIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + syIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + syIdx * xCount +fzIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + fyIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + fyIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(fxIdx + syIdx * xCount +szIdx * xCount * yCount);
                mDynamicGrid[tag].insert(sxIdx + syIdx * xCount +szIdx * xCount * yCount);
                */
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

#include "ai_utils.h"
#include <algorithm>
#include <cmath>
namespace Lina { namespace Helpers { namespace AI{
    BehaviourStatus LinearSequence::run()
    {
        for (auto behaviour : mChildren)
        {
            if (behaviour->run() == BehaviourStatus::Fail)
                return BehaviourStatus::Fail;
        }
        return BehaviourStatus::Success;
    };
    BehaviourStatus LinearSelector::run()
    {
        for (auto behaviour : mChildren)
        {
            if (behaviour->run() == BehaviourStatus::Success)
                return BehaviourStatus::Success;
        }
        return BehaviourStatus::Fail;
    };

    void Graph::readFromFile()
    {
    }

    void Graph::readFromMesh()
    {
    }
    f32 Graph::heuristic(u32 idx1, u32 idx2)
    {
        return 
            ((Math::Vector3D{mNodes[idx1].first->x, mNodes[idx1].first->y, 
        mNodes[idx1].first->z}) - 
            (Math::Vector3D{mNodes[idx2].first->x, mNodes[idx2].first->y, 
        mNodes[idx2].first->z})).squaredMagnitude();
    }
    void Graph::sortPath(AStarNode* finalNode)
    {
        while(finalNode != nullptr)
        {
            mPath.push_back(finalNode->id);
            finalNode = finalNode->parent;
        }
        std::reverse(mPath.begin(), mPath.end());
    }
    void Graph::traverse()
    {
        mPath.clear();
        mVisited.clear();
        std::vector<u32> openPath = {};
        mVisited.resize(mNodes.size());
        for (int i = 0; i < mNodes.size(); i++)
        {
            mVisited[i] = false; 
            mNodes[i].first->localCost = INFINITY;
            mNodes[i].first->totalCost = INFINITY;
            mNodes[i].first->parent = nullptr;
        }
        mVisited[mStart] = true;
        openPath.push_back(mStart);
        mNodes[mStart].first->localCost = 0;
        mNodes[mStart].first->totalCost = heuristic(mStart, mEnd);
        while (!openPath.empty())
        {
            u32 min = 0;
            u32 openIdx = 0;
            f32 minCost = INFINITY;
            for (int i = 0; i < openPath.size(); i++)
            {
                if (mNodes[openPath[i]].first->totalCost < minCost)
                {
                    minCost = mNodes[openPath[i]].first->totalCost;
                    min = openPath[i];
                    openIdx = i;
                }
            }
            auto current = mNodes[min].first;
            if (min == mEnd) 
            {
                mVisited.clear();
                sortPath(current);
                return;
            }
            openPath.erase(openPath.begin() + openIdx);
            mVisited[min] = true;


            for (int i = 0; i < mNodes[min].second.size(); i++)
            {
                auto newId = mNodes[min].second[i].second->id;
                if (mVisited[newId])
                {
                    continue;
                }
                auto tempCost = current->totalCost + 
                    mNodes[min].second[i].first;
                if (std::find(openPath.begin(), openPath.end(), newId) ==
                        openPath.end())
                {
                    openPath.push_back(newId);
                }
                else if (tempCost >= mNodes[newId].first->totalCost)
                {
                    continue;
                }
                mNodes[newId].first->parent = current;
                mNodes[newId].first->localCost = tempCost; 
                mNodes[newId].first->totalCost = tempCost + heuristic(newId, mEnd);
            }
        }
    }
}}}

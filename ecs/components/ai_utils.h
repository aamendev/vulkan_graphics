#ifndef AI_UTILS_H
#define AI_UTILS_H
#include "../../types.h"
#include <cmath>
#include <vector>
namespace Lina { namespace Helpers { namespace AI{
    enum class BehaviourStatus
    {
        Fail,
        Success,
        Pending,
        None
    };
    class AIBehaviour 
    {
        public:
            virtual BehaviourStatus run() = 0;
    };
    class LinearSequence : public AIBehaviour
    {
        public:
            virtual BehaviourStatus run() override;
            inline void addChild(AIBehaviour* beh) {mChildren.push_back(beh);}
            inline void removeChild(u32 idx) 
            {
                mChildren.erase(mChildren.begin() + idx);
            }
        private:
            std::vector<AIBehaviour*> mChildren;
    };
    class LinearSelector : public AIBehaviour
    {
        public:
            virtual BehaviourStatus run() override;
            inline void addChild(AIBehaviour* beh) {mChildren.push_back(beh);}
            inline void removeChild(u32 idx) 
            {
                mChildren.erase(mChildren.begin() + idx);
            }
        private:
            std::vector<AIBehaviour*> mChildren;
    };


    struct AStarNode
    {
        u32 id = 0;
        b8 isObstacle = false;
        f32 x = 0.0f;
        f32 y = 0.0f;
        f32 z = 0.0f;
        f32 localCost = INFINITY;
        f32 totalCost = INFINITY; // heuristic + localCost 
        inline friend b8 operator<(AStarNode& g1, AStarNode& g2);
        AStarNode* parent = nullptr;
    };

    class Graph
    {
        public: 
            Graph(): mNodes({}), mVisited({}), mPath({}), mStart(0), mEnd(0){}
        public:
        void readFromFile();
        void readFromMesh();
        void traverse();

        public: // getters
        inline std::vector<u32> getPath() const {return mPath;}
        inline std::pair<AStarNode*, std::vector<std::pair<f32, AStarNode*>>>&
            operator[](int idx) {return mNodes[idx];}
        inline std::pair<AStarNode*, std::vector<std::pair<f32, AStarNode*>>>&
            at(int idx) {return mNodes[idx];}

        inline u32 getSize() const {return mNodes.size();}

        public: //setters
        inline void setStart(u32 s) {mStart = s;}
        inline void setEnd(u32 s) {mEnd = s;}
        inline void addNode(
                std::pair<AStarNode*, std::vector<std::pair<f32, AStarNode*>>> n)
        {
            mNodes.push_back(n);
        }


        private:
        f32 heuristic(u32 idx1, u32 idx);
        void sortPath(AStarNode*);

        private:
        std::vector<
            std::pair<AStarNode*, std::vector<
            std::pair<f32 , AStarNode*>>>> mNodes;
        std::vector<b8> mVisited;
        std::vector<u32> mPath;
        u32 mStart;
        u32 mEnd;
    };
    b8 operator<(AStarNode& g1, AStarNode& g2)
    {
        return g1.totalCost < g2.totalCost;
    }
}}}
#endif

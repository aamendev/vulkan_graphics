#ifndef AI_COMPONENT_H
#define AI_COMPONENT_H
#include "component.h"
#include "transform.h"
#include "ai_utils.h"
#include <functional>
namespace Lina { namespace ECS { namespace Components{
    class AI : public Component
    {
        public:// core Logic
            AI(Helpers::AI::AIBehaviour* beh = nullptr):
                mRootBehaviour(beh){}

            Helpers::AI::BehaviourStatus update();

            inline void setRootBehaviour(Helpers::AI::AIBehaviour* beh) 
            {mRootBehaviour = beh;}

            inline Helpers::AI::AIBehaviour* getRootBehaviour() {return mRootBehaviour;}

        private:
            Helpers::AI::AIBehaviour* mRootBehaviour;
    };
}}}
#endif

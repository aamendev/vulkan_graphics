#include "ai.h"

namespace Lina { namespace ECS { namespace Components{

    Helpers::AI::BehaviourStatus AI::update()
    {
        return mRootBehaviour->run();
    }
}}}

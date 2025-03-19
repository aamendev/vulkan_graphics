#include "spring_force.h"

namespace Lina { namespace Physics { namespace Forces {
    void Spring::calculateForce()
    {
        auto length = (*mAnchor - *mAttached).magnitude() - mRestLength;
        mForce = (*mAnchor - *mAttached).normalise() * (length * mK);
    }
}}}

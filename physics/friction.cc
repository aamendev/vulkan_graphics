#include "friction.h"
#include <cmath>

namespace Lina { namespace Physics{ namespace Forces {
    void Friction::calculateForce()
    {
        mForce = {0,0,0};
        if (mEnabled && mRb->isInContact())
        {
        auto rot = mRb->getRot();
        auto angles = rot.toAngles();
        Math::Vector3D norm = {sinf(angles.z) * cosf(angles.y), cosf(angles.z), sinf(angles.y)*sinf(angles.z)};
        norm = norm.normalise();
        auto netForce = mRb->getLinearAcceleration() * mRb->getMass();
        std::cerr << "Net: " << mRb->getLinearAcceleration() << '\n';
        auto normForce = fabs(netForce.dot(norm));
        mForce = mRb->getLinearVelocity().normalise() * (normForce * -mMuk);
        std::cerr << "Ff" << mForce << '\n';
        }
    }
}}}

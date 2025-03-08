#include "particle.h"

namespace Lina { namespace Physics {

    void Particle::update(f32 t)
    {
        if (mInverseMass <= 0) return;
        mPosition += (mVelocity * t +  mAcceleration * (0.5f * t * t));
        mVelocity += (mAcceleration * t);
        mVelocity *= mDamping;
        mAcceleration = (mForces * mInverseMass);
    }
}}

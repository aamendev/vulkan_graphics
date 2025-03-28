#include "particle.h"

namespace Lina { namespace Physics {

    void Particle::update(f32 t)
    {
        if (mInverseMass <= 0) return;
        mAcceleration += (mForces * mInverseMass);
        for (int i= 0; i < mVariableForces.size(); i++)
        {
            mVariableForces[i]->calculateForce();
            mAcceleration += (mVariableForces[i]->getForce() * mInverseMass);
        }

        mPosition += (mVelocity * t +  mAcceleration * (0.5f * t * t));
        mVelocity += (mAcceleration * t);
        mVelocity *= mDamping;
        mAcceleration = (mForces * mInverseMass);
    }
}}

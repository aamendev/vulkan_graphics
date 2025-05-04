#include "circular_particle_system.h"

namespace Lina { namespace Physics {
    void CircularParticleSystem::init()
    {
        mSettings.scale = {mRadius, mRadius, mRadius};
        mParticles.resize(mSettings.particleCount);
        mSettings.lifeTimes.resize(mSettings.particleCount);
        mSettings.localElapse.resize(mSettings.particleCount);
        mSettings.rotations.resize(mSettings.particleCount);
        for (u32 i = 0; i < mSettings.particleCount; i++)
        {
            resetparticle(i);
        }
    }

    void CircularParticleSystem::update(f32 t)
    {
        if (isEnabled() && (mSettings.duration == 0.0f || mElapsedTime < mSettings.duration))
        {
            mElapsedTime += t; 
            for (int i = 0; i < mSettings.particleCount; i++)
            {
                mSettings.localElapse[i] += t;
                if (mSettings.localElapse[i] >= mSettings.lifeTimes[i])
                {
                    resetparticle(i);
                }
                mParticles[i].update(t);
            }
        }

    }
    void CircularParticleSystem::resetparticle(u32 idx)
    {

        mSettings.localElapse[idx] = 0.0f;;
        mParticles[idx].setPosition({
                Core::Utils::randFloat(mSettings.centre.x - mRadius, mSettings.centre.x + mRadius),
                mSettings.centre.y,
                Core::Utils::randFloat(mSettings.centre.z - mRadius, mSettings.centre.z + mRadius)
                });

        mParticles[idx].setVelocity(
                {
                Core::Utils::randFloat(mSettings.minVelocity.x, mSettings.maxVelocity.x),
                Core::Utils::randFloat(mSettings.minVelocity.y, mSettings.maxVelocity.y),
                Core::Utils::randFloat(mSettings.minVelocity.z, mSettings.maxVelocity.z),
                }
                );
        mSettings.lifeTimes[idx] = Core::Utils::randFloat(0, mSettings.maxLifeTime);

            mSettings.rotations[idx] = 
            {
            Core::Utils::randFloat(mSettings.minRotations.x, mSettings.maxRotations.x),
            Core::Utils::randFloat(mSettings.minRotations.y, mSettings.maxRotations.y),
            Core::Utils::randFloat(mSettings.minRotations.z, mSettings.maxRotations.z)
            };
    }
}}

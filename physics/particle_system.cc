#include "particle_system.h"
#include <cstdlib>

namespace Lina { namespace Physics {
    void ParticleSystem::init()
    {
        mParticles.reserve(mSpecs.particleCount);
        mSpecs.lifeTimes.reserve(mSpecs.particleCount);
        mSpecs.localElapse.resize(mSpecs.particleCount);
        mSpecs.rotations.resize(mSpecs.particleCount);
        for (int i = 0; i < mSpecs.particleCount; i++)
        {
            mParticles.emplace_back(
                    1, 
                    Math::Point3D{
                    randFloat(mSpecs.centre.x - mSpecs.radius, mSpecs.centre.x + mSpecs.radius),
                    mSpecs.centre.y,
                    randFloat(mSpecs.centre.z - mSpecs.radius, mSpecs.centre.z + mSpecs.radius),
                    },
                    Math::Vector3D{
                    randFloat(mSpecs.minVelocity.x, mSpecs.maxVelocity.x),
                    randFloat(mSpecs.minVelocity.y, mSpecs.maxVelocity.y),
                    randFloat(mSpecs.minVelocity.z, mSpecs.maxVelocity.z),
                    },
                    Math::Vector3D{0, 0, 0},
                    Math::Vector3D{0, 0, 0},
                    1.0f
                    );
            mSpecs.lifeTimes[i] = randFloat(0, mSpecs.maxLifeTime);
            mSpecs.rotations[i] = 
            {
                randFloat(mSpecs.minRotations.x, mSpecs.maxRotations.x),
            randFloat(mSpecs.minRotations.y, mSpecs.maxRotations.y),
            randFloat(mSpecs.minRotations.z, mSpecs.maxRotations.z)
            };
        }
    }

    void ParticleSystem::update(f32 t)
    {
        if (isEnabled() && (mSpecs.duration == 0.0f || mElapsedTime < mSpecs.duration))
        {
            mElapsedTime += t; 
            for (int i = 0; i < mSpecs.particleCount; i++)
            {
                mSpecs.localElapse[i] += t;
                if (mSpecs.localElapse[i] >= mSpecs.lifeTimes[i])
                {
                    resetparticle(i);
                }
                mParticles[i].update(t);
            }
        }
    }
    void ParticleSystem::resetparticle(u32 idx)
    {

        mSpecs.localElapse[idx] = 0.0f;;
        mParticles[idx].setPosition({
                randFloat(mSpecs.centre.x - mSpecs.radius, mSpecs.centre.x + mSpecs.radius),
                mSpecs.centre.y,
                randFloat(mSpecs.centre.z - mSpecs.radius, mSpecs.centre.z + mSpecs.radius)
                });

        mParticles[idx].setVelocity(
                {
                randFloat(mSpecs.minVelocity.x, mSpecs.maxVelocity.x),
                randFloat(mSpecs.minVelocity.y, mSpecs.maxVelocity.y),
                randFloat(mSpecs.minVelocity.z, mSpecs.maxVelocity.z),
                }
                );
        mSpecs.lifeTimes[idx] = randFloat(0, mSpecs.maxLifeTime);

            mSpecs.rotations[idx] = 
            {
            randFloat(mSpecs.minRotations.x, mSpecs.maxRotations.x),
            randFloat(mSpecs.minRotations.y, mSpecs.maxRotations.y),
            randFloat(mSpecs.minRotations.z, mSpecs.maxRotations.z)
            };
    }
}}

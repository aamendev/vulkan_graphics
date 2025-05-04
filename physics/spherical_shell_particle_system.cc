#include "spherical_shell_particle_system.h"
namespace Lina { namespace Physics{
    void SphericalShellParticleSystem::init()
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

    void SphericalShellParticleSystem::update(f32 t)
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

    void SphericalShellParticleSystem::resetparticle(u32 idx)
    {

        auto x = Core::Utils::randFloat(-1, 1);
        auto y = Core::Utils::randFloat(-1, 1);
        auto randSgn = (rand() % 2) == 0;
        auto zMag = sqrtf(1 - x*x - y*y);

        x = x * mRadius + mSettings.centre.x;
        y = y * mRadius + mSettings.centre.y;
        auto z = (zMag * randSgn - zMag * !randSgn) * mRadius + mSettings.centre.z;

        mSettings.localElapse[idx] = 0.0f;
        mParticles[idx].setPosition({
                x, y, z
                });

        Math::Vector3D v = 
        {
            Core::Utils::randFloat(mSettings.minVelocity.x, mSettings.maxVelocity.x),
            Core::Utils::randFloat(mSettings.minVelocity.y, mSettings.maxVelocity.y),
            Core::Utils::randFloat(mSettings.minVelocity.z, mSettings.maxVelocity.z),
        };

        if (mIsRadialVelocity)[[likely]]
        {
            v = 
                ((mSettings.centre - mParticles[idx].getPos()).normalise()) *
        //        v.normalise()).normalise()*
                v.magnitude();
        }
        mParticles[idx].setVelocity(std::move(v));
        mSettings.lifeTimes[idx] = Core::Utils::randFloat(0, mSettings.maxLifeTime);

        mSettings.rotations[idx] = 
        {
            Core::Utils::randFloat(mSettings.minRotations.x, mSettings.maxRotations.x),
            Core::Utils::randFloat(mSettings.minRotations.y, mSettings.maxRotations.y),
            Core::Utils::randFloat(mSettings.minRotations.z, mSettings.maxRotations.z)
        };
    }
}}

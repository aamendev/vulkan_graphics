#ifndef SPHERICAL_SHELL_PARTICLE_SYSTEM_H
#define SPHERICAL_SHELL_PARTICLE_SYSTEM_H
#include "particle_system.h"
namespace Lina { namespace Physics {
    class SphericalShellParticleSystem : public ParticleSystem
    {
        public:
            SphericalShellParticleSystem(): mRadius(1.0f), mIsRadialVelocity(true){}
        public:
            virtual void init() override;
            virtual void update(f32 t) override;
        public:
            inline void setRadius(f32 r) {mRadius = r;}
            inline void setRadial() {mIsRadialVelocity = true;}
            inline void setNotRadial() {mIsRadialVelocity = false;}
            inline void toggleRadial() {mIsRadialVelocity = !mIsRadialVelocity;}
        public:
            inline f32 getRadius(f32 r) {return mRadius;}
            inline b8 isRadial() {return mIsRadialVelocity;}
        private:
            virtual void resetparticle(u32 idx) override;
        private:
            f32 mRadius;
            b8 mIsRadialVelocity;
    };
}}
#endif

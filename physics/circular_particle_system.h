#ifndef CIRCULAR_PARTICLE_SYSTEM_H
#define CIRCULAR_PARTICLE_SYSTEM_H
#include "particle_system.h"

namespace Lina { namespace Physics {
    class CircularParticleSystem : public ParticleSystem
    {
        public:
            virtual void init() override;
            virtual void update(f32 t) override;
        public:
            inline void setRadius(f32 r) {mRadius = r;}
        public:
            inline f32 getRadius(f32 r) {return mRadius;}
        private:
            virtual void resetparticle(u32 idx) override;
        private:
            f32 mRadius;
    };
}}
#endif

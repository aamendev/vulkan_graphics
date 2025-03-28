#ifndef GRAVITY_H
#define GRAVITY_H
#include "../types.h"
#include "../Math/math.h"
#include "force.h"
#include "particle.h"
#include <functional>

namespace Lina { namespace Physics{ namespace Forces {
    class Gravity : public Force
    {
        public:
            Gravity(f64 G = 1.0, u32 mainBody = 0, f32 md = 0.01f, f32 mxd = 1000.0f): 
                mG(G), mParticles({}),
                mMainBody(mainBody), mMinDistance(md), mMaxDistance(mxd)
                {
                    mOnNearReject = defaultOnNearReject;
                    mOnFarReject = defaultOnFarReject;
                }

            virtual void calculateForce() override;

            Math::Vector3D calculateSingle(u32 idx);

            inline void addBody(Particle* a) 
            {mParticles.push_back(a);}

            inline void addForce(Math::Vector3D&& f) {mForce += f;}

        public:
            inline void setG(f64 G) {mG = G;}
            inline void setMinDistance(f32 r) {mMinDistance = r;}
            inline void setMaxDistance(f32 r) {mMaxDistance = r;}
            inline void setMainBody(u32 mb) {mMainBody = mb;}
            inline void setOnNearReject(
                    std::function<void(Gravity* g, Particle* p0, Particle* p)> onReject)
            {
                mOnNearReject = onReject;
            }

            inline void setOnFarReject(
                    std::function<void(Gravity* g, Particle* p0, Particle* p)> onReject)
            {
                mOnFarReject = onReject;
            }

        private:
            static void defaultOnNearReject(Gravity* g, Particle* p0, Particle* p){}

            static void defaultOnFarReject(Gravity* g, Particle* p0, Particle* p){}
        private:
            std::vector<Particle*> mParticles;
            std::function<void(Gravity*, Particle*, Particle*)> mOnNearReject;
            std::function<void(Gravity*, Particle*, Particle*)> mOnFarReject;
            f64 mG;
            u32 mMainBody;
            f32 mMinDistance;
            f32 mMaxDistance;
    };
}}}
#endif

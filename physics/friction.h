#ifndef FRICTION_H
#define FRICTION_H
#include "../types.h"
#include "../Math/math.h"
#include "force.h"
#include "rigidbody.h"
#include <functional>

namespace Lina { namespace Physics{ namespace Forces {
    class Friction : public Force
    {
        public:
            Friction(f64 muk = 0.2f, f64 mus = 0.2f): 
                mMus(mus), mMuk(muk), mRb(nullptr)
                {
                }

            virtual void calculateForce() override;
        public:
            inline void setMuk(f64 muk) {mMuk = muk;}
            inline void setMus(f64 mus) {mMus = mus;}
            inline void setRigidBody(RigidBody* rb) {mRb = rb;}
        private:

        private:
            f64 mMuk;
            f64 mMus;
            RigidBody* mRb;
    };
}}}
#endif

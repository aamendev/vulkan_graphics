#ifndef FORCE_RB_SYSTEM_H
#define FORCE_RB_SYSTEM_H
#include "../types.h"
#include "../Math/math.h"
#include "rigidbody.h"
#include "force.h"
namespace Lina { namespace Physics {
    class ForceRbSystem 
    {
        public:
            ForceRbSystem(RigidBody* rb = nullptr, Force* f = nullptr):
                mRb(rb), mForce(f){}
        public:
        public:
           // inline void setRigidBody(RigidBody* rb) {mRb = rb;}
            //inline void setForce(Force* f) {mForce = f;}
        private:
            RigidBody* mRb;
            Force* mForce;
    };
}}
#endif


#include "rigidbody.h"

namespace Lina { namespace Physics {

    void RigidBody::update(f32 t)
    {
        if (mRb.particle->getInverseMass() <= 0) return;

        for (auto& [f, p] : mRb.forces)
        {
            mRb.particle->addVariableForce(f);
            auto torque = (p).cross(f->getForce());
            mRb.angularAcceleration += torque * mRb.invInertiaTensor;
        }
        for (auto& [f, p] : mRb.impulses)
        {
            f->calculateForce();
            auto imp = f->getForce();
            mRb.particle->applyImpulse(imp);
            auto torque = (p).cross(f->getForce());
            mRb.angularAcceleration += torque * mRb.invInertiaTensor;
        }
        mRb.particle->update(t);

        auto currRotation = mRb.rotation.toAngles();
         auto update = (mRb.angularVelocity * t 
                + mRb.angularAcceleration * (0.5f * t *t));

         currRotation.x += update.x;
         currRotation.y += update.y;
         currRotation.z += update.z;
         mRb.rotation = Math::Quatrenion::angleToQuat(currRotation).normalise();
         mRb.angularVelocity += mRb.angularAcceleration * t;
        mRb.angularVelocity *= mRb.angularDamping;
        auto inTens = mRb.invInertiaTensor.inverse();
        inTens = mRb.rotation.getRotationMatrix() * inTens 
            * mRb.rotation.getRotationMatrix().transpose();
        mRb.invInertiaTensor = inTens.inverse();
        mRb.angularAcceleration = {0, 0, 0};
    }
}}

#ifndef PARTICLE_H
#define PARTICLE_H
#include "../types.h"
#include "../Math/math.h"
namespace Lina { namespace Physics {
    class Particle 
    {
        public:
            Particle(f32 m = 1.0f, Math::Point3D&& p = {0, 0, 0}, 
                    Math::Vector3D&& v = {0, 0 ,0}, Math::Vector3D&& a = {0, 0, 0},
                    Math::Vector3D&& f = {0, 0, 0},
                    f32 d = 1.0f): 
                mInverseMass(1 / m), mPosition(p), mVelocity(v), mAcceleration(a),
                mDamping(d), mForces(f) {}
        public:
            void update(f32 t);

            inline void applyForce(const Math::Vector3D& f) {mForces += f;}
            inline void resetForce() {mForces = {0, 0, 0};}
            inline void applyImpulse(const Math::Vector3D& f) 
            {mAcceleration += (f * mInverseMass);}
        public:
            inline void setForces(const Math::Vector3D& f) {mForces = f;}
            inline void setPosition(Math::Point3D&& p) {mPosition = p;}
            inline void setVelocity(Math::Vector3D&& v) {mVelocity = v;}
            inline void setAcceleration(Math::Vector3D&& a) {mAcceleration = a;}
            inline void setMass(f32 m) {mInverseMass = (1 / m);}
            inline void setDamping(f32 d) {mDamping = d;}
            inline void setInverseMass(f32 minv) {mInverseMass = minv;}
        public:
            inline const Math::Point3D& getPos() const {return mPosition;}
            inline const Math::Vector3D& getVelocity() const {return mVelocity;}
            inline const Math::Vector3D& getAcc() const {return mAcceleration;}
            inline f32 getMass() const {return (1 / mInverseMass);}
            inline f32 getInverseMass() const {return mInverseMass;}
        private:
  //          void calculateVelocity();
   //         void calculatePosition();
        private:
            Math::Point3D mPosition;
            Math::Vector3D mVelocity;
            Math::Vector3D mAcceleration;
            Math::Vector3D mForces;
            f32 mInverseMass;
            f32 mDamping;
            //f32 mDampingPower;
    };
}}
#endif

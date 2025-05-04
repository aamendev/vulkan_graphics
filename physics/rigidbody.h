#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#include "../types.h"
#include "../Math/math.h"
#include "force.h"
#include "particle.h"
namespace Lina { namespace Physics {
    struct RbSetting
    {
        Particle* particle = nullptr;
        Math::Quatrenion rotation = {0,0,0, 1};
        Math::Vector3D angularVelocity = {0,0,0};
        Math::Vector3D angularAcceleration {0,0,0};
        Math::Matrix3D invInertiaTensor = {0,0,0, 0,0,0, 0,0,0};
        std::vector<std::pair<Force*, Math::Point3D>> forces = {};
        std::vector<std::pair<Force*, Math::Point3D>> impulses = {};
        f32 angularDamping = 1.0f;
    };
    class RigidBody 
    {
        public:
            RigidBody(RbSetting rb = {}): mRb(rb), mEnabled(true), mInContact(false){}
        public:
            void loadSetting(RbSetting&rb) {mRb = rb;}
            void update(f32 t);
            inline void applyForce(Force* f, Math::Point3D&& p)
            {mRb.forces.emplace_back(f, p);}
            inline void applyImpulse(Force* f, Math::Point3D&& p)
            {mRb.impulses.emplace_back(f, p);}
            inline void applyImpulse(Math::Vector3D& f)
            {mRb.particle->applyImpulse(f);}
            inline void applyForce(Math::Vector3D&f)
            {
                mRb.particle->applyForce(f);
            }
            /*inline void applyForce(const Math::Vector3D& f) {mForces += f;}
            inline void addVariableForce(Force* f) {mVariableForces.push_back(f);}
            inline void resetForce() {mForces = {0, 0, 0};}*/
            //inline void applyImpulse(const Math::Vector3D& f) 
            //{mAcceleration += (f * mInverseMass);}
        public:
            //inline void setForces(const Math::Vector3D& f) {mForces = f;}
            inline void setPosition(Math::Point3D&& p) {mRb.particle->setPosition(std::move(p));}
            inline void updatePosition(Math::Point3D&& p) {mRb.particle->updatePosition(std::move(p));}
            inline void setLinearVelocity(Math::Vector3D&& v) {mRb.particle->setVelocity(std::move(v));}
            inline void updateLinearVelocity(Math::Vector3D&& v) {mRb.particle->updateVelocity(std::move(v));}
            inline void setLinearAcceleration(Math::Vector3D&& v) {mRb.particle->setAcceleration(std::move(v));}
            inline void setAngularVelocity(Math::Vector3D&& v) {mRb.angularVelocity = v;}
            inline void setAngularAcceleration(Math::Vector3D&& v) {mRb.angularAcceleration = v;}
            inline void setMass(f32 m) {mRb.particle->setMass(m);}
            inline void setInertiaTensor(Math::Matrix3D& m) {mRb.invInertiaTensor = m.inverse();}
            inline void setInverseInertiaTensor(Math::Matrix3D& m) {mRb.invInertiaTensor = m;}
            inline void setLinearDamping(f32 d) {mRb.particle->setDamping(d);}
            inline void setAngularDamping(f32 d) {mRb.angularDamping = d;}
            inline void setInverseMass(f32 minv) {mRb.particle->setInverseMass(minv);}
            inline void enable() {mEnabled = true;}
            inline void disable() {mEnabled = false;}
            inline void toggleEnable() {mEnabled = !mEnabled;}
            inline void disableForce(int idx) {mRb.forces[idx].first->disable();}
            inline void enableForce(int idx) {mRb.forces[idx].first->enable();}
            inline void inContact() {mInContact = true;}
            inline void notInContact() {mInContact = false;}
        public:
            inline const Math::Point3D& getPos() const {return mRb.particle->getPos();}
            inline const Math::Quatrenion& getRot() const {return mRb.rotation;}
            inline const Math::Vector3D& getLinearVelocity() const {return mRb.particle->getVelocity();}
            inline const Math::Vector3D& getLinearAcceleration() const {return mRb.particle->getAcc();}
            inline const Math::Vector3D& getAngularVelocity() const {return mRb.angularVelocity;}
            inline const Math::Vector3D& getAngularAcceleration() const {return mRb.angularAcceleration;}
            inline f32 getMass() const {return mRb.particle->getMass();}
            inline f32 getInverseMass() const {return mRb.particle->getInverseMass();}
            inline b8 isEnabled() const {return mEnabled;}
            inline b8 isInContact() const {return mInContact;}
            std::vector<std::pair<Force*, Math::Point3D>> getForces() {return mRb.forces;}
        private:
            RbSetting mRb;
            b8 mEnabled;
            b8 mInContact;
    };
}}
#endif

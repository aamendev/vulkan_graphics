#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "particle.h"
#include "../core/utils.h"
namespace Lina { namespace Physics {
    class ParticleSystem
    {
        struct Specs
        {
            // General Properties
            f32 duration = 0.0f;
            u32 particleCount = 4;
            f32 radius = 5.0f;
            Math::Vector3D velocity = {0.0, 0.0, 0.0};
           // Math::Vector3D normal = {0, 1, 0};
            Math::Point3D centre = {0, 0, 0};
            b8 enabled = true;

            // Particle Properties
            Math::Vector3D minVelocity = {0, 2.0f, 0};
            Math::Vector3D maxVelocity = {0, 10.0f, 0};
            std::vector<f32> lifeTimes = {};
            std::vector<f32> localElapse = {};
            f32 maxLifeTime = 1.0f;
            std::vector<Math::EulerAngles> rotations = {};
            Math::EulerAngles maxRotations = {PI / 4, PI / 4, PI / 4};
            Math::EulerAngles minRotations = {-PI / 4, -PI / 4, -PI / 4};
        };
        public:
            ParticleSystem(Specs&& s): mSpecs(s), mParticles({}){}
            ParticleSystem(): mParticles({}) {}
            void init();
            void update(f32 t);
        public:
            //inline void addParticle(const Particle& p) {mParticles.push_back(p);}
        public:
        inline const std::vector<Particle>& getParticles() const {return mParticles;}
        inline f32 getRadius() const {return mSpecs.radius;}
        inline const Math::Point3D& getPosition() const {return mSpecs.centre;}
        inline const Math::Vector3D& getVelocity() const {return mSpecs.velocity;}
        inline f32 getDuration() const {return mSpecs.duration;}
        inline u32 getParticleCount() const {return mSpecs.particleCount;}
        inline const std::vector<Math::EulerAngles>& getRotations() const {return mSpecs.rotations;}

        inline const Math::Vector3D& getMinV() const 
        {return mSpecs.minVelocity;}

        inline const Math::Vector3D& getMaxV() const
        {return mSpecs.maxVelocity;}

        inline b8 isEnabled() const {return mSpecs.enabled;}

        public:
        inline void setSpec(const Specs& s) {mSpecs = s;}
        inline void setRadius(f32 r) {mSpecs.radius = r;}
        inline void setPosition(const Math::Point3D& c) {mSpecs.centre = c;}
        inline void setVelocity(const Math::Vector3D& v) {mSpecs.velocity = v;}
        inline void setDuration(f32 d) {mSpecs.duration = d;}
        inline void setParticleCount(u32 c) {mSpecs.particleCount = c;}
        inline void setMinV(const Math::Vector3D& v) {mSpecs.minVelocity = v;}
        inline void setMaxV(const Math::Vector3D& v) {mSpecs.maxVelocity = v;}
        inline void setMaxLifeTime(f32 t) {mSpecs.maxLifeTime = t;}
        inline void setMinRotation(const Math::EulerAngles& e) {mSpecs.minRotations = e;}
        inline void setMaxRotation(const Math::EulerAngles& e) {mSpecs.maxRotations = e;}
        inline void enable() {mSpecs.enabled = false;}
        inline void disable() {mSpecs.enabled = true;}
        inline void toggle() {mSpecs.enabled = !mSpecs.enabled;}
        
        private:
            void resetparticle(u32 idx);
        private:
            std::vector<Particle> mParticles; 
            Specs mSpecs;
            f32 mElapsedTime;
            
    };
}}
#endif

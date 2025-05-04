#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H
#include "particle.h"
#include "../core/utils.h"
namespace Lina { namespace Physics {
    struct ParticleSystemSettings
    {
        // General Properties
        f32 duration = 0.0f;
        u32 particleCount = 4;
        //f32 radius = 5.0f;
        Math::Vector3D scale = {1.0f, 1.0f, 1.0f};
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
    class ParticleSystem
    {
        public:
            virtual ~ParticleSystem() = default;
            virtual void init() = 0;
            virtual void update(f32 t) = 0;
        public:
            //inline void addParticle(const Particle& p) {mParticles.push_back(p);}
        public:
        inline const std::vector<Particle>& getParticles() const {return mParticles;}
        inline const Math::Vector3D& getScale() const {return mSettings.scale;}
        inline const Math::Point3D& getPosition() const {return mSettings.centre;}
        inline const Math::Vector3D& getVelocity() const {return mSettings.velocity;}
        inline f32 getDuration() const {return mSettings.duration;}
        inline u32 getParticleCount() const {return mSettings.particleCount;}
        inline const std::vector<Math::EulerAngles>& getRotations() const {return mSettings.rotations;}

        inline const Math::Vector3D& getMinV() const 
        {return mSettings.minVelocity;}

        inline const Math::Vector3D& getMaxV() const
        {return mSettings.maxVelocity;}

        inline b8 isEnabled() const {return mSettings.enabled;}

        public:
        inline void setSettings(const ParticleSystemSettings& s) {mSettings = s;}
        inline void setScale(Math::Vector3D&& s) {mSettings.scale = s;}
        inline void setPosition(const Math::Point3D& c) {mSettings.centre = c;}
        inline void setVelocity(const Math::Vector3D& v) {mSettings.velocity = v;}
        inline void setDuration(f32 d) {mSettings.duration = d;}
        inline void setParticleCount(u32 c) {mSettings.particleCount = c;}
        inline void setMinV(const Math::Vector3D& v) {mSettings.minVelocity = v;}
        inline void setMaxV(const Math::Vector3D& v) {mSettings.maxVelocity = v;}
        inline void setMaxLifeTime(f32 t) {mSettings.maxLifeTime = t;}
        inline void setMinRotation(const Math::EulerAngles& e) {mSettings.minRotations = e;}
        inline void setMaxRotation(const Math::EulerAngles& e) {mSettings.maxRotations = e;}
        inline void enable() {mSettings.enabled = false;}
        inline void disable() {mSettings.enabled = true;}
        inline void toggle() {mSettings.enabled = !mSettings.enabled;}
        
        protected:
            virtual void resetparticle(u32 idx) = 0;
        protected:
            std::vector<Particle> mParticles; 
            ParticleSystemSettings mSettings;
            f32 mElapsedTime;
    };
}}
#endif

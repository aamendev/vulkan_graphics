#ifndef CHARACTER_CONTROLLER_H
#define CHARACTER_CONTROLLER_H
#include "system.h"
#include "../components/transform.h"
#include "../../events/all_events.h"
namespace Lina { namespace ECS { 
    class CharacterController: public System
    {
        public:
        CharacterController(Components::Transform* trans): mTrans(trans),
        mVelocity({0, 0, 0}), mIsGrounded(false) {}
        virtual void update() override;
        virtual void enable() override {}
        virtual void disable() override {}

        void setVelocity(Math::Vector3D v);
        void setGrounded(b8 g) {mIsGrounded = g;}
        void setRotationVelocity(Math::EulerAngles v);
        void updateVelocity(Math::Vector3D v) {mVelocity += v;}
        void setYVelocity(f64 vy) {mVelocity.y = vy;}

        const Math::Vector3D& getVelocity() const {return mVelocity;}
        b8 isGrounded() {return mIsGrounded;}
        private:
        Components::Transform* mTrans; 
        Math::Vector3D mVelocity;
        Math::EulerAngles mRotationVelocity;
        b8 mIsGrounded;
    };
}}
#endif

#include "character_controller.h"
namespace Lina { namespace ECS { 
    void CharacterController::update()
    {
        
        mTrans->translate(mVelocity);
        mTrans->rotate(mRotationVelocity);
    }

    void CharacterController::setVelocity(Math::Vector3D v)
    {
        mVelocity = v;
    }

    void CharacterController::setRotationVelocity(Math::EulerAngles v)
    {
        mRotationVelocity = v;
    }
}}

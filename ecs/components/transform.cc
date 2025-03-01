#include "transform.h"

namespace Lina { namespace ECS { namespace Components{
    Transform::Transform(float x, float y, float z, 
            float rx, float ry , float rz)
    {
        mPosition = {x, y, z};
        mRotation = Math::Quatrenion::angleToQuat({rx, ry, rz}); 
    }

    Transform::Transform(Math::Point3D pos, 
            Math::EulerAngles rot) : mPosition(pos)
    {
        mRotation = Math::Quatrenion::angleToQuat(rot);
    }

    void Transform::setPosition(Math::Point3D pos)
    {
        mPosition = pos;
    }

    void Transform::setRotation(Math::EulerAngles rot)
    {
        mRotation = Math::Quatrenion::angleToQuat(rot);
    }

    void Transform::setRotation(Math::Quatrenion q)
    {
        mRotation = q.normalise();
    }

    void Transform::translate(Math::Vector3D v)
    {
        mPosition += v;
    }

    void Transform::rotate(Math::EulerAngles r)
    {
       mRotation = (mRotation * Math::Quatrenion::angleToQuat(r)).normalise();
    }

    const Math::EulerAngles Transform::getRotationAngles() const
    {
        return mRotation.toAngles();
    }

    Math::Transform4D Transform::getMatrix() const
    {
        auto rotMat = mRotation.getRotationMatrix();
        return Math::Util::transMatrix(std::move(rotMat), mPosition);
    }
}}}

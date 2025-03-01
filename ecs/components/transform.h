#ifndef TRANSFORM_COMPONENT_H 
#define TRANSFORM_COMPONENT_H
#include "component.h"
namespace Lina { namespace ECS { namespace Components{
    class Transform : public Component
    {
        public:
            Transform() : mPosition({0, 0, 0}), mRotation(0, 0, 0, 1), mScale(1, 1, 1){}
            Transform(Math::Point3D pos, Math::Quatrenion rot): 
                mPosition(pos), mRotation(rot){}

            Transform(float x, float y, float z, float rx, float ry, float rz);
            Transform(Math::Point3D pos, Math::EulerAngles rot);
        
            void setPosition(Math::Point3D);
            void setRotation(Math::EulerAngles);
            void setRotation(Math::Quatrenion);
            void setScale(Math::Vector3D v) {mScale = v;};

            void translate(Math::Vector3D);
            void scale(Math::Vector3D v) {mScale.x *= v.x; mScale.y *= v.y; mScale.z *= v.z;}
            void rotate(Math::EulerAngles);
            void rotate(Math::Quatrenion);


        public:
            ComponentType getType() {return getStaticType();}
            const Math::Point3D& getPosition() const {return mPosition;}
            const Math::Quatrenion& getRotation() const {return mRotation;}
            const Math::Vector3D& getScale() const {return mScale;}
            const Math::EulerAngles getRotationAngles() const; 

            Math::Transform4D getMatrix() const;
            
        private:
            static ComponentType getStaticType() {return ComponentType::Transform;}
        private:
            Math::Point3D mPosition;
            Math::Quatrenion mRotation;
            Math::Vector3D mScale;
    };
}}}
#endif

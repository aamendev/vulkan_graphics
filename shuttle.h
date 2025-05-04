#ifndef SHUTTLE_H
#define SHUTTLE_H
#include "./Math//math.h"
#include "Math/Matrix3D.h"
#include "Math/Point3D.h"
#include "Math/Quatrenion.h"
#include "Math/Vector3D.h"
#include "events/all_events.h"
#include "events/events.h"
#include "events/key.h"
#include "events/mouse.h"
namespace Lina{ namespace Graphics{
    class Shuttle
    {
        public:
            Shuttle();

            void setPosition(float x, float y, float z);
            void setPosition(Math::Point3D& p);
            inline void setSpeed(f32 s) {mSpeed = s;}
            inline void setSenitivity(f32 s) {mSensitivity = s;}
            inline void setRotation(Math::Quatrenion&& quat) {mRotation = quat;}
            inline void setRotation(f32 h, f32 v) {mHangle = h; mVangle = v;};
            inline void rotate(f32 h, f32 v) {mHangle += h; mVangle += v;}
            void setRotation(Math::EulerAngles&& angle);
            inline void enableRotations() {mEnableRotations = true;}
            inline void disableRotations() {mEnableRotations = false;}
            inline void toggleRotations() {mEnableRotations = !mEnableRotations;}

            inline void enableMovement() {mEnableMovement = true;}
            inline void disableMovement() {mEnableMovement = false;}
            inline void toggleMovement() {mEnableMovement = !mEnableMovement;}

        public:
            inline const Math::Point3D& getPos() const {return mPos;}
            inline const Math::Vector3D& getTarget() const {return mTarget;}
            inline const Math::Vector3D& getUp() const {return mUp;}
            inline const Math::Quatrenion& getRotation() const {return mRotation;}
            inline const Math::EulerAngles getRotationAngle() const {
                return mRotation.toAngles();}

            inline const Math::Matrix3D getRotationMatrix() const {return mRotation.getRotationMatrix();}
            inline const Math::Matrix4D getRotationMatrix4D() const {return mRotation.getRotationMatrix4D();}
        public:
            void init();
            b8 onKeyDown(Events::KeyPress& e);
            b8 onKeyUp(Events::KeyRelease& e);
            b8 onMouseMove(Events::MouseMove& e);
            Math::Transform4D getMatrix();


        private:
            Math::Transform4D initMatrix(const Math::Vector3D& target, const Math::Vector3D& up);
        private:
            Math::Point3D mPos;
            Math::Vector3D mTarget;
            Math::Vector3D mUp;
            f32 mHangle = 0;
            f32 mVangle = 0;
            f32 mMousex = 0;
            f32 mMousey = 0;
            f32 mSensitivity = 5;
            float mSpeed = 0.1f;
            Math::Quatrenion mRotation;
            b8 mMouesRotation = false;
            Math::Transform4D mCamMatrix;
            b8 mEnableRotations = true;
            b8 mEnableMovement = false;
    };
}}
#endif

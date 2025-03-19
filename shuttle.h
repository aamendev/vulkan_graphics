#ifndef SHUTTLE_H
#define SHUTTLE_H
#include "./Math//math.h"
#include "events/all_events.h"
#include "events/events.h"
#include "events/key.h"
namespace Lina{ namespace Graphics{
    class Shuttle
    {
        public:
            Shuttle();

            void setPosition(float x, float y, float z);
            void setPosition(Math::Point3D& p);
            inline void setSpeed(f32 s) {mSpeed = s;}

            b8 update(Events::KeyPress& e);

            Math::Transform4D getMatrix();

            void init();

        private:
            Math::Transform4D initMatrix(const Math::Vector3D& target, const Math::Vector3D& up);
        private:
            Math::Point3D mPos;
            Math::Point3D mTarget;
            Math::Vector3D mUp;
            Math::Transform4D mCamMatrix;
            float mSpeed = 0.1f;
    };
}}
#endif

#include "../Math/math.h"
namespace Lina{ namespace Space{
    class Shuttle
    {
        public:
            Shuttle();

            void setPosition(float x, float y, float z);
            void setPosition(Math::Point3D& p);

            void update();

            Math::Transform4D getMatrix();

            void init();

        private:
            Math::Transform4D initMatrix(const Math::Vector3D& pos, const Math::Vector3D& target, const Math::Vector3D& up);
        private:
            Math::Point3D mPos;
            Math::Point3D mTarget;
            Math::Vector3D mUp;
            Math::Transform4D mCamMatrix;
            float mSpeed = 1.0f;
    };
}}


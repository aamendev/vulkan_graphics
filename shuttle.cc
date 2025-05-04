#include "shuttle.h"
#include "Math/Quatrenion.h"
#include "Math/Transform4D.h"
#include "Math/Utilities.h"
#include "Math/Vector3D.h"
#include "events/mouse.h"
#include "types.h"
namespace Lina{ namespace Graphics{
    Shuttle::Shuttle()
    {
        init();
    }

    void Shuttle::init()
    {
        mPos = Math::Point3D(0, 0, 0);
        mTarget = {0,0,1};
        mUp = {0, 1, 0};
    }

    void Shuttle::setPosition(float x, float y, float z)
    {
        mPos = Math::Point3D(x, y, z);
    }

    void Shuttle::setRotation(Math::EulerAngles&& angle)
    {
        mHangle = angle.x;
        mVangle = angle.y;
        auto q = Math::Quatrenion::angleToQuat(angle);
        mRotation = q;
    }

    void Shuttle::setPosition(Math::Point3D& p)
    {
        mPos = p;
    }

    b8 Shuttle::onKeyDown(Events::KeyPress& e)
    {
        auto k = e.key();
        if (mEnableMovement)[[unlikely]]
        {
            switch (k)
            {
                case Input::KeyCode::W:
                    mPos += (mTarget * mSpeed);
                    break;
                case Input::KeyCode::S:
                    mPos -= (mTarget * mSpeed);
                    break;

                case Input::KeyCode::A:
                    {
                        Math::Vector3D l = mTarget.cross(mUp);
                        l = 
                            mPos += l.normalise() * mSpeed;
                    }
                    break;
                case Input::KeyCode::D:
                    {
                        Math::Vector3D l = mTarget.cross(mUp);
                        mPos -= l.normalise() * mSpeed;
                    }
                    break;
                case Input::KeyCode::LeftBracket:
                    {
                        if (mSpeed > 0.10f)
                        {
                            mSpeed-= 0.01f;
                        }
                    }
                    break;
                case Input::KeyCode::RightBracket:
                    mSpeed+= 0.01f;
                    break;
                default:
                    {}
            }
        }
        if (mEnableRotations)[[likely]]
        {
            switch(k)
            {
                case Input::KeyCode::Down:
                    {

                        mVangle += mSensitivity;
                        if (mVangle > 360) mVangle -= 360;
                    }
                    break;

                case Input::KeyCode::Up:
                    {

                        mVangle -= mSensitivity;
                        if (mVangle < 0) mVangle += 360;
                    }
                    break;

                case Input::KeyCode::Right:
                    {
                        mHangle += mSensitivity;
                        if (mHangle > 360) mHangle -= 360;
                    }
                    break;

                case Input::KeyCode::Left:
                    {
                        mHangle -= mSensitivity;
                        if (mHangle < 0) mHangle += 360;
                    }
                    break;
                case Input::KeyCode::Minus:
                    {
                        mHangle = 0;
                        mVangle = 0;
                    }
                    break;
                case Input::KeyCode::D1:
                    {
                        std::cerr << "Looking Forward\n";
                        mTarget = {0, 0, 1};
                        mUp = {0, 1, 0};
                    }
                    break;

                case Input::KeyCode::D2:
                    {
                        std::cerr << "Looking Rightward\n";
                        mTarget = {1, 0, 0};
                        mUp = {0, 1, 0};
                    }
                    break;

                case Input::KeyCode::D3:
                    {
                        std::cerr << "Looking Left\n";
                        mTarget = {-1, 0, 0};
                        mUp = {0, 1, 0};
                    }
                    break;

                case Input::KeyCode::D4:
                    {
                        std::cerr << "Looking Backwards\n";
                        mTarget = {0, 0, -1};
                        mUp = {0, 1, 0};
                    }
                    break;
                case Input::KeyCode::D5:
                    {
                        std::cerr << "Looking Up\n";
                        mTarget = {0, -1, 0};
                        mUp = {0, 0, 1};
                    }
                    break;
                case Input::KeyCode::D6:
                    {
                        std::cerr << "Looking Down\n";
                        mTarget = {0, 1, 0};
                        mUp = {0, 0, -1};
                    }
                    break;
                case Input::KeyCode::Comma:
                    {
                        std::cerr << "Turning 90 Left\n";
                        mHangle -= 90;
                    }
                    break;

                case Input::KeyCode::Period:
                    {
                        std::cerr << "Turning 90 Right\n";
                        mHangle += 90;
                    }
                    break;
                default:
                    {}
            }
        }
        return true;
    }
    b8 Shuttle::onMouseMove(Events::MouseMove& e)
    {
        if (mMouesRotation)
        {
            mHangle += (mSensitivity * (mMousex < e.x()) - mSensitivity * (mMousex > e.x()));
            mHangle += (-360 * (mHangle > 360) + 360 * (mHangle < 0));
            mVangle += (mSensitivity * (mMousey < e.y()) - mSensitivity * (mMousey > e.y()));
            mVangle += (-360 * (mVangle > 360) + 360 * (mVangle < 0));
            mMousex = e.x();
            mMousey = e.y();
        }
        return true;
    }
    Math::Transform4D Shuttle::initMatrix(const Math::Vector3D& target, const Math::Vector3D& up)
    {

        const Math::Vector3D& n = target;
        const Math::Vector3D& u = up.cross(n).normalise();
        Math::Vector3D v = n.cross(u);
        Math::Transform4D camRot (u.x, u.y, u.z, 0,
                v.x, v.y, v.z, 0,
                n.x, n.y, n.z, 0);
        Math::Transform4D camTrans (1, 0, 0, -mPos.x,
                0, 1, 0, -mPos.y,
                0, 0, 1, -mPos.z);
        mHangle = 0;
        mVangle = 0;
        return camRot * camTrans;
    }

    Math::Transform4D Shuttle::getMatrix()
    {
        // mUp = {0, 1, 0};
        //mTarget = {0, 0, 1};

        mTarget = mTarget.rotate(mHangle * PI / 180, mUp).normalise();
        mUp = mUp.cross(mTarget).normalise();

        mTarget = mTarget.rotate(-mVangle * PI / 180, mUp).normalise();
        mUp = mTarget.cross(mUp).normalise();
        return initMatrix(mTarget, mUp);
    }
}}


#include "shuttle.h"
#include "Math/Vector3D.h"
namespace Lina{ namespace Graphics{
    Shuttle::Shuttle()
    {
        init();
    }
    void Shuttle::init()
    {
        mPos = Math::Point3D(0, 0, 0);
        mTarget = Math::Point3D(0,0,1);
        mUp = Math::Util::yAxis();
    }
    void Shuttle::setPosition(float x, float y, float z)
    {
        mPos = Math::Point3D(x, y, z);
    }
    void Shuttle::setPosition(Math::Point3D& p)
    {
        mPos = p;
    }

    b8 Shuttle::update(Events::KeyPress& e)
    {
        auto k = e.key();
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
                    l = l.normalise() * mSpeed;
                    mPos += l;
                }
                break;
            case Input::KeyCode::D:
                {
                    Math::Vector3D l = mTarget.cross(mUp);
                    l = l.normalise() * mSpeed;;
                    mPos -= l;
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
            default:
                {}
        }
        return true;
    }
    Math::Transform4D Shuttle::initMatrix(const Math::Vector3D& target, const Math::Vector3D& up)
    {
        Math::Vector3D n = target.normalise();
        Math::Vector3D u = up.cross(n).normalise();
        Math::Vector3D v = n.cross(u);
        Math::Transform4D camTrans (u.x, u.y, u.z, -mPos.x,
                v.x, v.y, v.z, -mPos.y,
                n.x, n.y, n.z, -mPos.z);
        return camTrans;
    }

    Math::Transform4D Shuttle::getMatrix()
    {
        return initMatrix(mTarget, mUp);
    }
}}


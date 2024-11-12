#include "shuttle.h"
namespace Lina{ namespace Space{
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

    void Shuttle::update()
    {
       /*if (Manager::Input::isKeyPressed(Key::W))
       {
            mPos.z += (0.01 * mSpeed);
       }
       else if (Manager::Input::isKeyPressed(Key::S))
       {
            mPos.z -= (0.01 * mSpeed);
       }
       else if (Manager::Input::isKeyPressed(Key::A))
       {
            mPos.x -= (0.01 * mSpeed);
       }
       else if (Manager::Input::isKeyPressed(Key::D))
       {
            mPos.x += (0.01 * mSpeed);
       }
       else if (Manager::Input::isKeyPressed(Key::LeftBracket))
       {
            mSpeed -= 1.0;
       }
       else if (Manager::Input::isKeyPressed(Key::RightBracket))
       {
            mSpeed += 1.0;
       }*/
    }

    Math::Transform4D Shuttle::initMatrix(const Math::Vector3D& pos, const Math::Vector3D& target, const Math::Vector3D& up)
    {
        Math::Transform4D camTrans;
        Math::Vector3D n = target.normalise();
        Math::Vector3D u = up.cross(n).normalise();
        Math::Vector3D v = n.cross(u);
       camTrans(0,0) =  u.x; camTrans(0,1) = u.y; camTrans(0,2) = u.z; camTrans(0,3) = -pos.x;
       camTrans(1,0) =  v.x; camTrans(1,1) = v.y; camTrans(1,2) = v.z; camTrans(1,3) = -pos.y;
       camTrans(2,0) =  n.x; camTrans(2,1) = n.y; camTrans(2,2) = n.z; camTrans(2,3) = -pos.z;
       return camTrans;
    }

    Math::Transform4D Shuttle::getMatrix()
    {
       return initMatrix(mPos, mTarget, mUp);
    }
}}


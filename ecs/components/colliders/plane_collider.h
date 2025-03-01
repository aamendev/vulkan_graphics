#ifndef PLANE_COLLIDER_H
#define PLANE_COLLIDER_H
#include "collider.h"

namespace Lina { namespace ECS { namespace Components {
    class PlaneCollider : public Collider
    {
        public:
            PlaneCollider() = default;
            PlaneCollider(std::string tag): mWidth(10), mLength(5) 
            {
                mCallDefaults = true;
                mTag = tag;
                mRecentCollisions = {};
                mCenter = {0, 0, 0};
                mGeometry = ColliderGeometry::Plane;
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
                mResolveCallback = staticDefaultOnResolve;
            }
        PlaneCollider(std::string tag, f32 h, f32 r, Math::Point3D c): 
            mLength(r), mWidth(h)
        {
            mCallDefaults = true;
            mTag = tag;
            mRecentCollisions = {};
            mCenter = c;
            mGeometry = ColliderGeometry::Plane;
        };

        b8 checkCollision(Collider* c) override {return true;};
        virtual void onCollisionEnter(Collider* c) override;
        virtual void onCollisionExit(Collider* c) override;
        virtual void onCollisionPersist(Collider* c) override;
        virtual void onResolve(Collider* c) override;

        public:
        void setPosition(Math::Point3D c) {mCenter = c;}
        void setWidth(f32 h) {mWidth = h;}
        void setLength(f32 r) {mLength = r;}

        public:
        f32 getWidth() const {return mWidth;}
        f32 getLength() const {return mLength;}
        const Math::Vector3D& getCenter() const {return mCenter;}
        private:
        static void staticDefaultOnCollisionEnter(Collider* c0, Collider* c1) {}
        static void staticDefaultOnResolve(Collider* c0, Collider* c1) {}
        static void staticDefaultOnCollisionExit(Collider* c0, Collider* c1) {}
        static void staticDefaultOnCollisionPersist(Collider* c0, Collider* c1) {}
     //   b8 planeCylinderColision(Collider* c);
        private:
        f32 mLength;
        f32 mWidth;
    };
}}}
#endif

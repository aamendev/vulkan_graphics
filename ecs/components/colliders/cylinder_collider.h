#ifndef CYLINDER_COLLIDER_H
#define CYLINDER_COLLIDER_H
#include "collider.h"
#include "collision_funcs.h"

namespace Lina { namespace ECS { namespace Components { namespace Colliders{
    class Cylinder : public Collider
    {
        public:
            Cylinder() = default;
            Cylinder(std::string tag): mHeight(10), mRadius(5)
            {
                mTransformed = false;
                mTag = tag;
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Cylinder;
                mRotation = {0, 0, 0};

                mCenter = {0, 0 ,0};
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mResolveCallback = staticDefaultOnResolve;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
            }

        Cylinder(std::string tag, f32 h, f32 r, Math::Point3D c): 
            mRadius(r), mHeight(h){
                mTag = tag;
                mRotation = {0, 0, 0};
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Cylinder;

                mCenter = c;
            };

        virtual b8 checkCollision(Collider* c) override;

        virtual void onCollisionEnter(Collider *c) override; 
        virtual void onCollisionExit(Collider* c) override;
        virtual void onCollisionPersist(Collider* c) override;
        virtual void onResolve(Collider* c) override;
        virtual void computeBVH() override;

        public:
     //   void setPosition(Math::Point3D c) {mCenter = c;}
        void setHeight(f32 h) {mHeight = h;}
        void setRadius(f32 r) {mRadius = r;}

        public:
        f32 getHeight() const {return mHeight;}
        f32 getRadius() const {return mRadius;}
        const Math::Vector3D& getCenter() const {return mCenter;}
        
        public:
            virtual Math::Point3D furthestPoint(const Math::Vector3D &d) override;
            virtual void computeBoundingBox() override;
        private:

        private:
        void defaultOnCollisionEnter(Collider* c);
        void defaultOnCollisionExit(Collider* c);
        void defaultOnResolve(Collider* c);
        void defaultOnCollisionPersist(Collider* c);

        private:
        static void staticDefaultOnCollisionEnter(Collider* c1, Collider* c2)
        {
        //    ((Cylinder*)c1)->defaultOnCollision(c2);
        }
        static void staticDefaultOnResolve(Collider* c1, Collider* c2)
        {
         //   ((Cylinder*)c1)->defaultOnCollision(c2);
        }
        static void staticDefaultOnCollisionExit(Collider* c1, Collider* c2)
        {
         //   ((Cylinder*)c1)->defaultOnCollision(c2);
        }
        static void staticDefaultOnCollisionPersist(Collider* c1, Collider* c2)
        {
         //   ((Cylinder*)c1)->defaultOnCollision(c2);
        }
        private:
        f32 mHeight;
        f32 mRadius;
    };
}}}}
#endif

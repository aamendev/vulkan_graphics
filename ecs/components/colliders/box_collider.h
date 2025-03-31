#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H
#include "collider.h"
#include "collision_funcs.h"

namespace Lina { namespace ECS { namespace Components { namespace Colliders{
    class Box : public Collider
    {
        public:
            Box() = default;
            Box(std::string tag, const std::pair<Math::Point3D, Math::Point3D>& p);
            Box(std::string tag): mHeight(1), mLength(1), mWidth(1)
            {
                mTag = tag;
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Box;

                mCenter = {0, 0 ,0};
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mResolveCallback = staticDefaultOnResolve;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
            }

        Box(std::string tag, f32 l, f32 w, f32 h, Math::Point3D c): 
            mLength(l), mWidth(w), mHeight(h){
                mTag = tag;
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Cylinder;
                mCenter = c;
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mResolveCallback = staticDefaultOnResolve;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
            };
        void initWithExtents(std::string tag, 
                const std::pair<Math::Point3D, Math::Point3D>& p); 

        virtual b8 checkCollision(Collider* c) override;

        virtual void onCollisionEnter(Collider *c) override; 
        virtual void onCollisionExit(Collider* c) override;
        virtual void onCollisionPersist(Collider* c) override;
        virtual void onResolve(Collider* c) override;
        virtual void computeBVH() override;

        public:
        inline void setPosition(Math::Point3D c) {mCenter = c;}
        inline void setLength(f32 l) {mLength = l;}
        inline void setWidth(f32 w) {mWidth = w;}
        inline void setHeight(f32 h) {mHeight = h;}

        public:
        inline f32 getHeight() const {return mHeight;}
        inline f32 getLength() const {return mLength;}
        inline f32 getWidth() const {return mWidth;}
        inline const Math::Vector3D& getCenter() const {return mCenter;}
        public:
            virtual Math::Point3D furthestPoint(const Math::Vector3D &d) override;
            virtual void computeBoundingBox() override;

        private:
        void defaultOnCollisionEnter(Collider* c){};
        void defaultOnCollisionExit(Collider* c){};
        void defaultOnResolve(Collider* c){};
        void defaultOnCollisionPersist(Collider* c){};
        

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
        f32 mLength;
        f32 mWidth;
        f32 mHeight;
        Helpers::Collisions::Simplex mSimplex;
    };
}}}}
#endif


#ifndef MESH_COLLIDER_H
#define MESH_COLLIDER_H
#include "box_collider.h"
#include "collider.h"
#include "collision_funcs.h"
#include <iterator>
#include <utility>
namespace Lina{ namespace ECS { namespace Components { namespace Colliders{
    class Mesh: public Collider
    {
        public:
            Mesh() : mVertices({})
            {
                mTag = "";
                mEnabled = true;
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Mesh;
                mCollisionInfo = {false, {0, 0, 0}, 0};

                mCenter = {0, 0 ,0};
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mResolveCallback = staticDefaultOnResolve;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
            };
            Mesh(std::string tag): mVertices({})
            {
                mTag = tag;
                mEnabled = true;
                mRecentCollisions = {};
                mCallDefaults = true;
                mGeometry = ColliderGeometry::Mesh;

                mCenter = {0, 0 ,0};
                mCollisionInfo = {false, {0, 0, 0}, 0};
                mCollisionEnterCallback = staticDefaultOnCollisionEnter;
                mResolveCallback = staticDefaultOnResolve;
                mCollisionExitCallback = staticDefaultOnCollisionExit;
                mCollisionPersistCallback = staticDefaultOnCollisionPersist;
            }
            
        public:
        virtual void onCollisionEnter(Collider *c) override; 
        virtual void onCollisionExit(Collider* c) override;
        virtual void onCollisionPersist(Collider* c) override;
        virtual void onResolve(Collider* c) override;
        virtual void computeBVH() override;

        public:
        virtual void computeBoundingBox() override;
        virtual void computeBBCompound(); 
            virtual Math::Point3D furthestPoint(const Math::Vector3D& d) override;
            virtual b8 checkCollision(Collider* b) override 
            {
                mCollisionInfo =  Helpers::Collisions::gjkAndInfo(this, b);

                return mCollisionInfo.collided;
            }
            inline void setVertices(std::vector<f32> verts) {mVertices = verts;}
            inline void setIndices(std::vector<u32> verts) {mIndices = verts;}
            inline void setCollisionInfo(const Helpers::Collisions::Info& inf)
            {mCollisionInfo = inf;}
        private:
            std::pair<Math::Point3D, Math::Point3D> computeBoundingBox(u32 idxBegin, u32 idxEnd);
            void combineBVHs(u32 idx0, u32 idx1, std::vector<Collider::BVH*>& mbvhs);
            void combineBVHs(Collider::BVH* b1, Collider::BVH* b2);
            Collider::BVH* computeBVH(u32 idxBegin, u32 idxEnd);  
            Collider::BVH * computeBVH(u32 idxBegin);
        private:
            b8 gjk(Collider* a, Collider* b);
            Math::Vector3D support(Collider* a, Collider* b, const Math::Vector3D& d);
            b8 checkNextSimplex(Math::Vector3D& v);

            b8 checkLine(Math::Vector3D& d);
            b8 checkTri(Math::Vector3D& d);
            b8 checkTetra(Math::Vector3D& d);
        private:
            void defaultOnCollisionEnter(Collider* c){};
            void defaultOnCollisionExit(Collider* c){};
            void defaultOnResolve(Collider* c){};
            void defaultOnCollisionPersist(Collider* c){};
        static void staticDefaultOnCollisionEnter(Collider* c1, Collider* c2)
        {
            //std::cerr << "CollisionEnter\n";
        }
        static void staticDefaultOnResolve(Collider* c1, Collider* c2)
        {
            //std::cerr << "CollisionResolve\n";
         
        }
        static void staticDefaultOnCollisionExit(Collider* c1, Collider* c2)
        {
            //std::cerr << "CollisionExit\n";
        }
        static void staticDefaultOnCollisionPersist(Collider* c1, Collider* c2)
        {

            //std::cerr << "CollisionPersist\n";
        }

        public:
            const Helpers::Collisions::Info& getInfo() const {return mCollisionInfo;}
        private:
            std::vector<f32> mVertices;
            std::vector<u32> mIndices;
            Helpers::Collisions::Simplex mSimplex;
            Helpers::Collisions::Info mCollisionInfo;
    };
}}}}
#endif

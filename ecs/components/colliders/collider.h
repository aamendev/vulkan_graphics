#ifndef COLLIDER_H
#define COLLIDER_H
#include "../../../types.h"
#include <functional>
#include <unordered_map>
#include "../component.h"
namespace Lina{ namespace ECS { namespace Components {
    enum class CollisionStatus
    {
        None,
        Enter,
        Exit,
        Persist,
    }; class Collider : public Component
    {
        public:
            virtual ~Collider() = default;
        public:
            virtual b8 checkCollision(Collider* c) = 0;
            virtual void onCollisionEnter(Collider*) = 0;
            virtual void onCollisionExit(Collider*) = 0;
            virtual void onCollisionPersist(Collider*) = 0;
            virtual void onResolve(Collider*) = 0;


            inline virtual void setOnCollisionEnter(std::function<void (Collider*, Collider*)> f)
            {mCollisionEnterCallback = f;};

            inline virtual void setOnResolve(std::function<void(Collider*, Collider*)> f)
            {mResolveCallback = f;}

            inline virtual void setOnCollisionExit(std::function<void(Collider*, Collider*)>f)
            {mCollisionExitCallback = f;}

            inline virtual void setOnCollisionPersist(std::function<void(Collider*, Collider*)>f)
            {mCollisionPersistCallback = f;}

            inline void setPosition(const Math::Point3D& newPos) {mCenter = newPos;}
            inline void setScale(const Math::Vector3D& s) {mScale = s;}
            inline void setScale(f32 s) {mScale  = {s, s, s};}
            inline void scale(f32 s) {mScale *= s;}
            inline virtual void setStatus(
                    std::string s, CollisionStatus stat) {mRecentCollisions[s] = stat;}

            inline void setRotation(Math::EulerAngles a) {mRotation = a;}
            inline virtual void removeStatus(std::string s) {mRecentCollisions.erase(s);}

            inline virtual void setCallDefaults(b8 c) {mCallDefaults = c;}

        public:
            const Math::Vector3D& getPosition() const {return mCenter;}
            const Math::Vector3D& getScale() const {return mScale;}
            ColliderGeometry getColliderGeometry() const {return mGeometry;}

            CollisionStatus getStatus(std::string s) const {
                auto it = mRecentCollisions.find(s);
                if (it ==  mRecentCollisions.end())
                    return CollisionStatus::None;
            return it->second;}

            const std::string& getTag() const {return mTag;}
            b8 isCallDefault() const {return mCallDefaults;}

        public:
            virtual Math::Point3D furthestPoint(const Math::Vector3D& d) = 0;

        protected:
            std::function<void (Collider* c, Collider* c2)> mCollisionEnterCallback;
            std::function<void (Collider* c, Collider* c2)> mResolveCallback;
            std::function<void (Collider* c, Collider* c2)> mCollisionExitCallback;
            std::function<void (Collider* c, Collider* c2)> mCollisionPersistCallback;
            std::string mTag;
            ColliderGeometry mGeometry;
            Math::Point3D mCenter;                
            Math::Vector3D mScale;
            Math::EulerAngles mRotation;
            b8 mCallDefaults;
            std::unordered_map<std::string, CollisionStatus> mRecentCollisions;
    };
}}}
#endif

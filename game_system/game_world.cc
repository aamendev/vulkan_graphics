#include "game_world.h"
#include <cstdlib>

namespace Lina { namespace GameSystem {
    
    void GameWorld::fetchComponents()
    {
        for (auto& [tag, value] : mObjects)
        {
            auto& reqs = value.getAddRequests();
            auto& removeReqs = value.getRemoveRequests();

            for (int j = 0; j < reqs.size(); j++)
            {
                switch(reqs[j])
                {
                    case ComponentType::CylinderCollider:
                        {
                            ECS::Components::Colliders::Cylinder cyl(tag);
                            mCylinderColliderComponents[tag] = cyl;
                        }
                        break;

                    case ComponentType::MeshCollider:
                        {
                            ECS::Components::Colliders::Mesh cyl(tag);
                            mMeshColliderComponents[tag] = cyl;
                        }
                        break;
                    case ComponentType::PlaneCollider:
                        {
                            ECS::Components::Colliders::Plane pl(tag);
                            mPlaneColliderComponents[tag] = pl;
                        }
                        break;
                    case ComponentType::AI:
                        {
                            ECS::Components::AI ai;
                            mAIComponents[tag] = ai;
                        }
                        break;
                    case ComponentType::Health:
                        {
                            ECS::Components::Health health;
                            mHealthComponents[tag] = health;
                        }
                        break;
                    case ComponentType::Transform:
                        {
                            ECS::Components::Transform trans;
                            mTransformComponents[tag] = trans;
                        }
                        break;
                    case ComponentType::Material:
                        {
                            ECS::Components::Material mat;
                            mMaterialComponents[tag] = mat;
                        }
                        break;
                    default: {}
                }
            }
            value.clearAddReqs();
            for (int j = 0; j < removeReqs.size(); j++)
            {
                switch(removeReqs[j])
                {
                    case ComponentType::CylinderCollider:
                            mCylinderColliderComponents.erase(tag);
                            break;
                    case ComponentType::PlaneCollider:
                        {
                            mPlaneColliderComponents.erase(tag);
                        }
                        break;
                    case ComponentType::Health:
                        {
                            mHealthComponents.erase(tag);
                        }
                        break;
                    case ComponentType::Transform:
                        {
                            mTransformComponents.erase(tag);
                        }
                        break;
                    case ComponentType::Material:
                        {
                            mMaterialComponents.erase(tag);
                        }
                        break;
                    default: {}
                }

            }
            value.clearRemoveReqs();
        }
    }

    void GameWorld::removeObject(std::string tag)
    {
        if (mHealthComponents.find(tag) != mHealthComponents.end())
        {
            mHealthComponents[tag].disable();
            mHealthComponents.erase(tag);
        }

        if (mTransformComponents.find(tag) != mTransformComponents.end())
        {

            mTransformComponents[tag].disable();
            mTransformComponents.erase(tag);
        }

        if (mCylinderColliderComponents.find(tag) != mCylinderColliderComponents.end())
        {
            mCylinderColliderComponents[tag].disable();
            //mCylinderColliderComponents.erase(tag);
        }

        if (mPlaneColliderComponents.find(tag) != mPlaneColliderComponents.end())
        {
            mPlaneColliderComponents[tag].disable();
            mPlaneColliderComponents.erase(tag);
        }

        if (mMaterialComponents.find(tag) != mMaterialComponents.end())
        {
            mMaterialComponents[tag].disable();
            mMaterialComponents.erase(tag);
        }

        if (mObjects.find(tag) != mObjects.end())
        {
            mObjects.erase(tag);
        }
    }
}}

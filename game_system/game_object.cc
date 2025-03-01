#include "game_object.h"
namespace Lina { namespace GameSystem {

    void GameObject::addComponent(ComponentType type)
    {
        mComponentAddRequests.push_back(type);
    }

    void GameObject::removeComponent(ComponentType type)
    {
        mComponentRemoveRequests.push_back(type);
    }

    void GameObject::resetComponents()
    {
       mComponentAddRequests.clear(); 
       mComponentRemoveRequests.clear(); 
    }
}}

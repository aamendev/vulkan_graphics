#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "../ecs/components/component.h"
namespace Lina { namespace GameSystem { 
    class GameObject
    {
        public:
            GameObject(std::string tag = ""): mTag(tag), 
            mComponentAddRequests({}), mComponentRemoveRequests({}){}

            void addComponent(ComponentType comp);
            void removeComponent(ComponentType comp);
            void resetComponents();
            inline void clearAddReqs() {mComponentAddRequests.clear();}
            inline void clearRemoveReqs() {mComponentRemoveRequests.clear();}

        public:
            const std::string& getTag() const {return mTag;}
            const std::vector<ComponentType>& getAddRequests() {
                return mComponentAddRequests;
            }

            const std::vector<ComponentType>& getRemoveRequests() {
                return mComponentRemoveRequests;
            }
        private:
            std::vector<ComponentType> mComponentAddRequests;
            std::vector<ComponentType> mComponentRemoveRequests;
            std::string mTag;
    };
}}
#endif

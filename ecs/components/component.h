#ifndef COMPONENT_H
#define COMPONENT_H
#include <string>
#include "../../types.h"
namespace Lina { namespace ECS { 
    class Component
    {
        public:
            const std::string& getTag() {return mTag;}
            virtual ComponentType getType() {return ComponentType::None;}
            virtual void enable() {mEnabled = true;}
            virtual void disable() {mEnabled = false;}
            virtual b8 isEnabled() {return mEnabled;}
        protected:
            std::string mTag;
            b8 mEnabled = true;
    };
}}
#endif

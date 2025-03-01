#ifndef SYSTEM_H
#define SYSTEM_H
#include "../../types.h"
namespace Lina { namespace ECS{
    class System 
    {
        public:
        virtual void update() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
    };
}}

#endif

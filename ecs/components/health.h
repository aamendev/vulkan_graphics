#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H
#include <string>
#include "../../types.h"
#include "component.h"
namespace Lina { namespace ECS { namespace Components{
    class Health : public Component
    {
        public:
            Health() : mHealth(100){}
            void takeDamage(f32 damage);
            void restore();
            void heal(f32 heal);
            void instantDeath();
        
            public:
            const f32& getHealth() const {return mHealth;}
        private:
            f32 mHealth;
    };
}}}
#endif

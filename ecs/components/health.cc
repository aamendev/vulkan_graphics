#include "health.h"

namespace Lina { namespace ECS { namespace Components{
    void Health::takeDamage(f32 damage)
    {
        mHealth -= damage;
        mHealth *= (mHealth > 0);
    }
    void Health::restore()
    {
       mHealth = 100; 
    }
    void Health::heal(f32 heal)
    {
        mHealth += heal;
        mHealth = mHealth * (mHealth <= 100) + 100 * (mHealth > 100);
    }
    void Health::instantDeath()
    {
        mHealth = 0;
    }
}}}

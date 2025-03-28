#ifndef FORCE_H
#define FORCE_H
#include "../Math/math.h"
#include "../types.h"
namespace Lina { namespace Physics {
    class Force 
    {
        public:
            virtual void calculateForce() = 0;

        public:
            inline virtual const Math::Vector3D& getForce() {return mForce;};
        public:
            inline void enable() {mEnabled = true;}
            inline void disable() {mEnabled = false;}
            inline void toggleEnable() {mEnabled = !mEnabled;}
        protected:
            Math::Vector3D mForce = {0, 0, 0};
            b8 mEnabled = true;
    };
}}
#endif

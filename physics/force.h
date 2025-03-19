#ifndef FORCE_H
#define FORCE_H
#include "../Math/math.h"
#include "../types.h"
namespace Lina { namespace Physics {
    class Force 
    {
        public:
        virtual const Math::Vector3D& getForce() {return mForce;};
        virtual void calculateForce() = 0;

        protected:
            Math::Vector3D mForce = {0, 0, 0};
    };
}}
#endif

#ifndef SPRING_FORCE_H
#define SPRING_FORCE_H
#include "force.h"

namespace Lina { namespace Physics{ namespace Forces {
    class Spring : public Force
    {
        public:
        Spring(f64 k = 1.0f, f64 l = 5.0f): mK(k), mRestLength(l), mAnchor(nullptr),
        mAttached(nullptr){}

        virtual void calculateForce() override;

        inline void setAnchor(Math::Point3D* a) {mAnchor = a;}
        inline void attachPoint(Math::Point3D* a) {mAttached = a;}
        inline void setSpringConstant(f64 k) {mK = k;}
        inline void setRestLength(f64 l) {mRestLength = l;}
        private:
            Math::Point3D* mAnchor; 
            Math::Point3D* mAttached;
            f64 mK;
            f64 mRestLength;
    };
}}}
#endif

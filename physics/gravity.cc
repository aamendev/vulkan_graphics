#include "gravity.h"


namespace Lina { namespace Physics{ namespace Forces {

    void Gravity::calculateForce() 
    {
        if (mEnabled)[[likely]]
        {
            mForce = {0, 0, 0};
            for (u32 i = 0; i < mParticles.size(); i++)
            {
                if (i == mMainBody || !mParticles[i]->isEnabled()) continue;

                auto diff = mParticles[i]->getPos() - mParticles[mMainBody]->getPos();
                f32 r2 = diff.squaredMagnitude();
                if (r2 >= mMinDistance && r2 <= mMaxDistance)
                {
                    mForce += 
                        diff.normalise() * 
                        (mG * 
                         (mParticles[i]->getMass() * mParticles[mMainBody]->getMass())
                         / diff.squaredMagnitude());
                }
                else if (r2 > mMaxDistance)[[likely]]
                {
                    mOnFarReject(this,mParticles[mMainBody], mParticles[i]);
                }
                else if (r2 < mMinDistance)
                {
                    mOnNearReject(this, mParticles[mMainBody], mParticles[i]);
                }
            }
        }
    }

    Math::Vector3D Gravity::calculateSingle(u32 idx)
    {
        auto diff = mParticles[idx]->getPos() - mParticles[mMainBody]->getPos();
        f32 r2 = diff.squaredMagnitude();
        return 
            diff.normalise() * 
            (mG * (mParticles[idx]->getMass() 
                   * mParticles[mMainBody]->getMass()) / diff.squaredMagnitude());
    }
}}}

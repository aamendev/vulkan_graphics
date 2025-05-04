#ifndef COLOR_MODIFIER_H
#define COLOR_MODIFIER_H
#include "modifier.h"
#define MAX_OCTAVES (64)
namespace Lina{ namespace Graphics{

    class ColModifier : public Modifier
    {
        public:
            virtual void fbm(f32 h, f32 lac, f32 octaves) override;
        public:
           inline const std::vector<f32>& getAlphas() const{return mAlphas;};
           inline const std::vector<f32>& getFullVertices() const{return mFullVertices;};
           void combineVertices();
        private:
            std::vector<f32> mAlphas;
            std::vector<f32> mFullVertices;
    };
}}
#endif

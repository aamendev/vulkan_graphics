#ifndef MATERIAL_H
#define MATERIAL_H
#include "../../types.h"
#include "component.h"
namespace Lina { namespace ECS { namespace Components {
    typedef struct RGBA
    {
        f32 r = 1.0f;
        f32 g = 1.0f;
        f32 b = 1.0f;
        f32 a = 1.0f;
    } RGBA;
    class Material : public Component
    {
        public:
            Material() = default;
            Material(MeshGeometry s, RGBA rgba = {1.0f, 1.0f, 1.0f, 1.0f}) : mShape(s),
            mColour(rgba){}

        public:
            inline void setColour(RGBA rgba) {mColour = rgba;}
            inline void setShape(MeshGeometry geo) {mShape = geo;}
            inline void setShader(u32 idx) {mShader = idx;}
        public:
            const RGBA& getColour() const {return mColour;}
            const MeshGeometry getShape() const {return mShape;}
            const u32 getShaderId() const {return mShader;}
        private:
            MeshGeometry mShape;
            RGBA mColour;
            u32 mShader;
    };
}}}
#endif

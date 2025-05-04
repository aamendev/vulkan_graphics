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
            mColour(rgba), mIsDynamic(false), mMultipleColours({}){}

        public:
            inline void setColour(RGBA rgba) {mColour = rgba;}
            inline void setShape(MeshGeometry geo) {mShape = geo;}
            inline void setShader(u32 idx) {mShader = idx;}
            inline void addColour(RGBA c) {mMultipleColours.push_back(c);}
            inline void setDynamic(b8 d) {mIsDynamic = d;}
        public:
            inline const RGBA& getColour() const {return mColour;}
            inline const RGBA* getMultipleColoursPointer() const {return mMultipleColours.data();}
            inline const MeshGeometry getShape() const {return mShape;}
            inline const u32 getShaderId() const {return mShader;}
            inline b8 isDynamic() {return mIsDynamic;}
        private:
            MeshGeometry mShape;
            RGBA mColour;
            std::vector<RGBA> mMultipleColours;
            u32 mShader;
            b8 mIsDynamic;
    };
}}}
#endif

#ifndef GEOMETRY_MODIFIER_H
#define GEOMETRY_MODIFIER_H
#include "modifier.h"
#define MAX_OCTAVES (64)
namespace Lina{ namespace Graphics{

    enum class ModifierNormals
    {
        Provided,
        Compute,
        Ignore
    };
    class GeoModifier : public Modifier
    {
        public:
            GeoModifier(): 
                mNormalsMode(ModifierNormals::Compute)
                {}
        public:
            virtual void fbm(f32 h, f32 lac, f32 octaves) override;
            void hybridMultiFractal(f32 h, f32 lac, f32 octaves, f32 offset);
            void warp(f32 h, f32 lac, f32 octaves, f32 dist);
           // void recalculateNormals();
            void combineVertices();
        public:
            std::vector<Math::Vector3D>& getNormals() {return mNormals;}
            std::vector<f32>& getFullVertices() {return mFullVertices;}
        public:
            inline void setNormalMode(ModifierNormals m) {mNormalsMode = m;}
            inline void setNormals(std::vector<Math::Vector3D>& verts) {mNormals = verts;}
            inline void setMulFactor(f32 f) {mMulFactor = f;}
        private:
            std::vector<Math::Vector3D> mNormals;
            std::vector<f32> mFullVertices;
            std::vector<u32> mIndices;
            ModifierNormals mNormalsMode;

    };
}}
#endif

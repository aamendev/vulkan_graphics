#ifndef TINY_BASED_H
#define TINY_BASED_H
#include  "../types.h"
#include "../third_party/tiny_obj_loader.h"
#include "../shapes/renderable.h"
namespace Lina { namespace Core {
    class TinyBasedObjLoader
    {
        public:
            TinyBasedObjLoader() = default;

            b8 load(std::string&& path);
        public:
            const std::vector<f32>& getFullVertices() const {return mFullVertices;};
            const std::vector<f32>& getPositions() const {return mVertices;}
            const std::vector<u32>& getIndices() const {return mIndices;}
            MeshMode getMeshMode() const {return mMode;}
        private:
            std::vector<f32> mVertices;
            std::vector<f32> mNormals;
            std::vector<f32> mTexture;
            std::vector<f32> mFullVertices;
            std::vector<u32> mIndices;
            std::vector<f32> mColors;
            //Graphics::Renderable mRenderable;
            MeshMode mMode = MeshMode::Pos3;
    };
}}
#endif

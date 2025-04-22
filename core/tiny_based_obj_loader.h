#ifndef TINY_BASED_H
#define TINY_BASED_H
#include  "../types.h"
#include "../third_party/tiny_obj_loader.h"
#include "../shapes/renderable.h"
#include "../ecs/components/ai_utils.h"
namespace Lina { namespace Core {
    class TinyBasedObjLoader
    {
        public:
            TinyBasedObjLoader() = default;

            b8 load(std::string&& path);
        public:
            inline void setNavPath(std::string&& path) {mNavPath = path;}
        public:
            const std::vector<f32>& getFullVertices() const {return mFullVertices;};
            const std::vector<f32>& getPositions() const {return mVertices;}
            const std::vector<u32>& getIndices() const {return mIndices;}
            MeshMode getMeshMode() const {return mMode;}
            Helpers::AI::Graph getGraph();
        private:
            b8 shares_edge(std::array<u32, 3>& a1, std::array<u32, 3>& a2);
            f32 distance(Helpers::AI::AStarNode* a1, Helpers::AI::AStarNode* a2);
        private:
            std::vector<f32> mVertices;
            std::vector<f32> mNormals;
            std::vector<f32> mTexture;
            std::vector<f32> mFullVertices;
            std::vector<u32> mIndices;
            std::vector<f32> mColors;
            std::string mPath;
            std::string mNavPath;
            //Graphics::Renderable mRenderable;
            MeshMode mMode = MeshMode::Pos3;
    };
}}
#endif

#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
#include "../types.h"
namespace Lina { namespace Core {
    class ObjLoader
    {
        public:
            ObjLoader() = default;

            b8 load(std::string&& path);
        public:
            const std::vector<f32>& getFullVertices() const {return mFullVertices;};
            const std::vector<f32>& getPositions() const {return mVertices;}
            const std::vector<u32>& getIndices() const {return mIndices;}
            MeshMode getMeshMode() const {return mMode;}
        private:
            void readIntoVector(std::string& l, std::vector<f32>*, b8 texture);
            void readFull(std::string& l);
            void readFile(std::string& path);
            void setIndices();
            void clearAll();
            void debug();
        private:
            std::vector<f32> mVertices;
            std::vector<f32> mNormals;
            std::vector<f32> mTexture;
            std::vector<f32> mFullVertices;
            std::vector<u32> mIndices;
            MeshMode mMode;
    };
}}
#endif

#pragma once
#include "common.h"
#include <fstream>
namespace Lina{ namespace Graphics{
    class Shader
    {
        public:
            void init(std::string&& vertexSource, std::string&& fragmentSource);
            void init(const std::string& vertexSource, const std::string& fragmentSource);
            std::vector<char> getVertexShader();
            std::vector<char> getFragmentShader();
        private:
            std::vector<char> readFileBinary(const std::string& shaderSource);
        private:
            std::vector<char> mVertexShader;
            std::vector<char> mFragmentShader;
    };
}}

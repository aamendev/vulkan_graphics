#pragma once
#include "common.h"
#include "types.h"
#include "uniform_buffer.h"
#include <fstream>
namespace Lina{ namespace Graphics{
    class Shader
    {
        public:

            void compileInit(
                    std::string&& vertexSource,
                    std::string&& fragmentSource);

            void compileInit(
                    const std::string& vertexSource, 
                    const std::string& fragmentSource);

            void init(std::string&& vertexSource, std::string&& fragmentSource, std::string&& ref);
            void init(const std::string& vertexSource, const std::string& fragmentSource, std::string&& ref);

            void addPushConstant(const PushConstant& ps);
            void addUniform(const Uniform& uniform);

        public:
            std::vector<char> getVertexShader();
            std::vector<char> getFragmentShader();

            u32 getBindingSize() {return mUniforms.size();}
            u32 getPushConstantSize() {return mPushConstant.size();}
            const std::vector<Uniform>& getUniforms() {return mUniforms;}
            const std::vector<PushConstant>& getPushConstants() {return mPushConstant;}
        private:
            std::vector<char> readFileBinary(const std::string& shaderSource);
        private:
            std::vector<char> mVertexShader;
            std::vector<char> mFragmentShader;
            std::vector<Uniform> mUniforms;
            std::vector<PushConstant> mPushConstant;
            std::string mReferenceName;
    };
}}

#pragma once
#include "common.h"
#include "types.h"
#include "uniform_buffer.h"
#include <fstream>
namespace Lina{ namespace Graphics{
    class Shader
    {
        public:
            Shader():
                mVertexShader({}),
                mFragmentShader({}),
                mDynamicUniformCount(0),
                mStaticUniformCount(0),
                mReferenceName(""),
                mUniforms({}),
                mPushConstant({}) {}

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

            inline u32 getBindingSize() {return mUniforms.size();}
            inline u32 getPushConstantSize() {return mPushConstant.size();}
            inline const std::vector<Uniform>& getUniforms() {return mUniforms;}
            inline const std::vector<PushConstant>& getPushConstants() {return mPushConstant;}
            inline u32 getDynamicUniformsCount() {return mDynamicUniformCount;}
            inline u32 getStaticUniformsCount() {return mStaticUniformCount;}
        private:
            std::vector<char> readFileBinary(const std::string& shaderSource);
        private:
            std::vector<char> mVertexShader;
            std::vector<char> mFragmentShader;
            std::vector<Uniform> mUniforms;
            std::vector<PushConstant> mPushConstant;
            std::string mReferenceName;
            u32 mDynamicUniformCount;
            u32 mStaticUniformCount;
    };
}}

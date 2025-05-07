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
                mPushConstant({}),
                mLoadMainTexture(false){}

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
            inline void addTexture(const TextureData& td) {mTextureData.push_back(td);}
            inline void combineUniforms()
            {
               mUniforms.reserve(mStaticUniforms.size() + mDynamicUniforms.size());
               for (auto& u : mStaticUniforms)
               {
                   mUniforms.push_back(u);
               }
               for (auto& u : mDynamicUniforms)
               {
                   mUniforms.push_back(u);
               }
            }
        public:
            inline void loadMainTexture() {mLoadMainTexture = true;}
            inline void notLoadMainTexture() {mLoadMainTexture = false;}
            inline void toggleLoadMainTexture() {mLoadMainTexture = !mLoadMainTexture;}

        public:
            std::vector<char> getVertexShader();
            std::vector<char> getFragmentShader();

            inline u32 getBindingSize() {return mStaticUniforms.size() + 
                mDynamicUniforms.size() + mTextureData.size();}

            inline u32 getUniformSize() {return mStaticUniforms.size() + 
                mDynamicUniforms.size();}

            inline u32 getPushConstantSize() {return mPushConstant.size();}
            inline const std::vector<Uniform>& getStaticUniforms() {return mStaticUniforms;}
            inline const std::vector<Uniform>& getDynamicUniforms() {return mDynamicUniforms;}
            inline std::vector<Uniform>& getUniforms()
            {
               return mUniforms;
            }
            inline std::vector<TextureData>& getTextureData()
            {
                return mTextureData;
            }
            inline u32 getTextureCount() {return mTextureData.size();}
            inline const std::vector<PushConstant>& getPushConstants() {return mPushConstant;}
            inline u32 getDynamicUniformsCount() {return mDynamicUniformCount;}
            inline u32 getStaticUniformsCount() {return mStaticUniformCount;}
            inline b8 checkLoadMainTexture() const {return mLoadMainTexture;}
        private:
            std::vector<char> readFileBinary(const std::string& shaderSource);
        private:
            std::vector<char> mVertexShader;
            std::vector<char> mFragmentShader;
            std::vector<Uniform> mStaticUniforms;
            std::vector<Uniform> mDynamicUniforms;
            std::vector<Uniform> mUniforms;
            std::vector<PushConstant> mPushConstant;
            std::vector<TextureData> mTextureData;
            std::string mReferenceName;
            u32 mDynamicUniformCount;
            u32 mStaticUniformCount;
            b8 mLoadMainTexture;
    };
}}

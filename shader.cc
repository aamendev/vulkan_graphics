#include "shader.h"
#include "types.h"
namespace Lina{ namespace Graphics{
    void Shader::init(std::string&& vertexSource, std::string&& fragmentSource,
            std::string&& ref)
    {
        mVertexShader = readFileBinary(vertexSource);
        mFragmentShader = readFileBinary(fragmentSource);
        mReferenceName = ref;
    }

    void Shader::init(const std::string& vertexSource, 
            const std::string& fragmentSource, std::string&& ref)
    {
        mVertexShader = readFileBinary(vertexSource);
        mFragmentShader = readFileBinary(fragmentSource);
        mReferenceName = ref;
    }
    
    void Shader::compileInit(std::string&& vertexSource, std::string&& fragmentSource)
    {

    }

    void Shader::compileInit(
            const std::string& vertexSource, 
            const std::string& fragmentSource)
    {

    }


    void Shader::addPushConstant(const PushConstant& ps)
    {
        mPushConstant.push_back(ps);
    }

    void Shader::addUniform(const Uniform& uniform)
    {
        mUniforms.push_back(uniform);
    }

    std::vector<char> Shader::readFileBinary(const std::string& shaderSource)
    {
        std::ifstream file(shaderSource, std::ios::ate | std::ios::binary);
        if (!file)
        {
            throw std::runtime_error("Failed to open");
        }
        u32 fileSize = (u32) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        return buffer;
    }
    std::vector<char> Shader::getVertexShader()
    {
       return mVertexShader;
    }

    std::vector<char> Shader::getFragmentShader()
    {
       return mFragmentShader;
    }
}}

#include "shader.h"
namespace Lina{ namespace Graphics{
    void Shader::init(std::string&& vertexSource, std::string&& fragmentSource)
    {
        mVertexShader = readFileBinary(vertexSource);
        mFragmentShader = readFileBinary(fragmentSource);
    }

    void Shader::init(const std::string& vertexSource, const std::string& fragmentSource)
    {
        mVertexShader = readFileBinary(vertexSource);
        mFragmentShader = readFileBinary(fragmentSource);
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

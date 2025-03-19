#include "obj_loader.h"
#include <algorithm>
#include <cstdio>
#include <string>

namespace Lina { namespace Core {
    void ObjLoader::debug()
    {
       auto f = freopen("../logs/errs.txt", "w", stderr);
        auto fullSize = 3 + 2 * (!mTexture.empty()) + 3 * (!mNormals.empty());
        std::cerr << "Vertices:\n {\n";
        for (int i = 0; i < mFullVertices.size(); i+= fullSize)
        {
            for (int j = 0; j < fullSize; j++)
            {
                std::cerr << mFullVertices[i + j] << ", ";
            }
            std::cerr << '\n';
        }

        std::cerr << "}\n\n";

        std::cerr << "Indices:\n {\n";

        for (int i = 0; i < mIndices.size(); i+= 3)
        {
            for (int j = 0; j < 3; j++)
            {
                std::cerr << mIndices[i + j] << ", ";
            }
            std::cerr << '\n';
        }
        std::cerr << "}\n\n";
        fclose(f);
    }
    b8 ObjLoader::load(std::string&& f)
    {
        readFile(f);
        mMode = (MeshMode)
                ((int)MeshMode::Pos3Tex2Norm3 * (!mTexture.empty() && !mNormals.empty())
                 + (int)MeshMode::Pos3Tex2 * (!mTexture.empty() && mNormals.empty())
                 + (int)MeshMode::Pos3 * (mTexture.empty() && mNormals.empty()));
        //debug();
        return true;
    }
    void ObjLoader::readFile(std::string& f)
    {
        FILE* stream = fopen(f.c_str(), "r");
        if (!stream)
        {
            std::cerr << "Failed to find file!\n";
            return;
        }

        enum DataType
        {
            None = -1,
            Vertices,
            Normals,
            Texture,
            Face,
        };
        char* buffer = NULL;
        size_t size = 0;

        while (getline(&buffer, &size, stream) != EOF)
        {
            std::string line(buffer);
            size_t pos = 0;
            if (line.find("#") == std::string::npos)
            {
                if ((pos = line.find("v")) != std::string::npos)
                {
                    if (line.find("t ") == pos + 1)
                    {
                        readIntoVector(line, &mTexture, true);
                        continue;
                    }
                    else if (line.find("n ") == pos + 1)
                    {
                        readIntoVector(line, &mNormals, false);
                        continue;
                    }
                    else
                    {
                        readIntoVector(line, &mVertices, false);
                        continue;
                    }
                }
                else if (line.find("f ") != std::string::npos)
                {
                    readFull(line);            
                    continue;
                }
                else 
                {
                    if (line.empty() || line == "\n")
                        continue;
                    //std::cerr << "Invalid Obj File!\n Line: " << line << '\n';
                    //clearAll();
                    continue;
                    break;
                }
            }
        }
        fclose(stream);
       // setIndices();
    }
    void ObjLoader::setIndices()
    {
        int divisor = 8 - 2 * (mTexture.empty()) - 3 * (mNormals.empty());
        mIndices.resize(mFullVertices.size() / divisor);
        for (int i = 0; i < mIndices.size(); i++)
        {
           mIndices[i] = i; 
        }
    }

    void ObjLoader::readIntoVector(std::string& line, std::vector<f32>* v, b8 texture)
    {
        int oldSize = v->size();
        auto currentIter = line.cbegin();
        while (currentIter != line.cend())
        {
            auto s = std::find_if(currentIter, line.cend(),
                    [](auto x){return (x >= '0' && x <= '9') || x == '-';});
            auto s2 = std::find(s, line.cend(), ' ');
            auto vt = std::stof(std::string(s, s2));
            v->push_back(vt); 
            currentIter = s2;
        }
        if (texture)
        {
            while ((v->size() - oldSize) % 3 > 0)
            {
                std::cerr << "In\n";
                v->push_back(1.0f);
            }
        }
    }

    void ObjLoader::readFull(std::string& line)
    {
        auto currentIter = line.cbegin();
        auto currentIndexSize = mIndices.size();
        auto faceCount = 0;
        while (currentIter != line.cend())
        {
            faceCount++;
            auto dataStart = std::find_if(currentIter, line.cend(),
                    [](auto x){return x >= '0' && x <= '9';});

            auto dataEnd = std::find(dataStart, line.cend(), ' ');
            auto vertexEnd = std::find(dataStart, dataEnd, '/');
            // v/v/v
            auto vt = 3 * (std::stoi(std::string(dataStart, vertexEnd)) - 1);
            for (int i = 0; i < 3; i++)
            {
                mFullVertices.push_back(mVertices[vt + i]);
            }
            if (vertexEnd != dataEnd)
            {
                auto textureEnd = std::find(vertexEnd + 1, dataEnd, '/');
                vt = 2 * (std::stoi(std::string(vertexEnd + 1, textureEnd)) - 1);
                for (int i = 0; i < 2; i++)
                {
                    mFullVertices.push_back(mTexture.at(vt + i));
                }
                if (textureEnd != dataEnd)
                {
                    auto normalEnd = std::find(textureEnd + 1, dataEnd, '/');
                    vt = 3 * (std::stoi(std::string(textureEnd + 1, normalEnd)) - 1);
                    for (int i = 0; i < 3; i++)
                    {
                        mFullVertices.push_back(mNormals.at(vt + i));
                    }
                }
            }
            currentIter = dataEnd;
        }
        for (int i = 0; i < faceCount - 2; i++)
        {
            mIndices.push_back(currentIndexSize);
            mIndices.push_back(currentIndexSize + i + 1);
            mIndices.push_back(currentIndexSize + i + 2);
        }
    }

    void ObjLoader::clearAll()
    {
        mVertices.clear();
        mIndices.clear();
        mNormals.clear();
        mTexture.clear();
        mFullVertices.clear();
    }
}}

#pragma once
#include "common.h"
namespace Lina{ namespace Graphics{
    enum class Format
    {
        FLOAT1 = VK_FORMAT_R32_SFLOAT,
        FLOAT2 = VK_FORMAT_R32G32_SFLOAT,
        FLOAT3 = VK_FORMAT_R32G32B32_SFLOAT,
        FLOAT4 = VK_FORMAT_R32G32B32A32_SFLOAT,

        UINT1 = VK_FORMAT_R32_UINT,
        UINT2 = VK_FORMAT_R32G32_UINT,
        UINT3 = VK_FORMAT_R32G32B32_UINT
    };
    struct VertexBufferElement
    {
        Format format;
        u32 location;
        static inline u32 getSizeOfFormat(Format format)
        {
            switch(format)
            {
                case Format::FLOAT1:
                case Format::FLOAT2:
                case Format::FLOAT3:
                case Format::FLOAT4:
                case Format::UINT1:
                case Format::UINT2:
                case Format::UINT3:
                    return 4;
                    break;
                default:
                    return 1;
            }
        }
        static inline u32 getCountOfFormat(Format format)
        {
            switch(format)
            {
            case Format::FLOAT1:
            case Format::UINT1:
                return 1;
                break;
            case Format::FLOAT2:
            case Format::UINT2:
                return 2;
                break;
            case Format::FLOAT3:
            case Format::UINT3:
                return 3;
                break;
            case Format::FLOAT4:
                return 4;
                break;
            default:
                return 1;
            }
        }
    };
    class VertexBufferLayout
    {
        public:
            VertexBufferLayout() : mStride(0) {}

            void push(Format format, u32 location)
            {
                mElements.push_back({format, location});
                mStride += VertexBufferElement::getCountOfFormat(format)
                    * VertexBufferElement::getSizeOfFormat(format);
            }

            /* Getters */
            inline std::vector<VertexBufferElement> getElements()
            {
                return mElements;
            }
            inline u32 getStride()
            {
                return mStride;
            }
        private:
            std::vector<VertexBufferElement> mElements;
            u32 mStride;
    };
}}
